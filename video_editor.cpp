#include "video_editor.h"

namespace std {
    // Расчет компоненты U для пикселя в формате YUV420
    uint8_t VideoEditor::UCalculation(array<uint8_t, 3> pixel) {
        uint16_t result = -0.14713 * pixel[RGB::kR] -
                          0.28886 * pixel[RGB::kG] +
                          0.436 * pixel[RGB::kB] + 128;
        if (result > 255)
            result = 255;
        return result;
    }

    // Расчет компоненты V для пикселя в формате YUV420
    uint8_t VideoEditor::VCalculation(array<uint8_t, 3> pixel) {
        uint16_t result = 0.615 * pixel[RGB::kR] -
                          0.51499 * pixel[RGB::kG] -
                          0.10001 * pixel[RGB::kB] + 128;
        if (result > 255)
            result = 255;
        return result;
    }

    // Редактирование видео в формате YUV420
    void VideoEditor::EditVideo(const char *path, size_t width, size_t height) {
        ifstream video_file(path, ios::binary);
        if (!video_file.is_open()) {
            cerr << "Ошибка открытия файла исходного видео." << endl;
        }
        std::ofstream save_file("../bus_edited.yuv", std::ios::binary);
        if (!save_file.is_open()) {
            cerr << "Ошибка открытия файла редактируемого видео." << endl;
        }

        size_t frame_size = width * height * 3 / 2; // Размер кадра в байтах для YUV420

        // Буфер для чтения кадра
        frame_data_.resize(frame_size);
        if (written_width_ == 0) {
            cerr << "Файл изображения не был сконвертирован, либо имеет неправильный размер." << endl;
        }
        u_shift_ = width * height;
        v_shift_ = width * height * 5 / 4;
        // Чтение и запись кадров
        while (video_file.read((char *) frame_data_.data(), frame_size)) {
            size_t frame_data_counter = 0;
            for (size_t i = 0; i < frame_size; ++i) {
                if (i % width < written_width_ && frame_data_counter < y_frame_.size()) {
                    frame_data_[i] = y_frame_[frame_data_counter++];
                }
                if (frame_data_counter == y_frame_.size())
                    break;
            }
            frame_data_counter = 0;
            for (size_t i = u_shift_; i < frame_size; ++i) {
                if ((i - u_shift_) % (width / 2) < written_width_ / 2 && frame_data_counter < u_frame_.size()) {
                    frame_data_[i] = u_frame_[frame_data_counter++];
                }
                if (frame_data_counter == y_frame_.size())
                    break;
            }
            frame_data_counter = 0;
            for (size_t i = v_shift_; i < frame_size; ++i) {
                if ((i - v_shift_) % (width / 2) < written_width_ / 2 && frame_data_counter < v_frame_.size()) {
                    frame_data_[i] = v_frame_[frame_data_counter++];
                }
                if (frame_data_counter == y_frame_.size())
                    break;
            }
            save_file.write((char *) frame_data_.data(), frame_size);
        }
        video_file.close();
    }

    // Подготовка кадра из BMP файла в формат YUV420
    void VideoEditor::PrepareFrame(BMPReader *bmp_reader) {
        size_t size = bmp_reader->bmp_info_header.width * bmp_reader->bmp_info_header.height;
        y_frame_.resize(size);
        written_width_ = bmp_reader->bmp_info_header.width;
        for (size_t i = 0; i < bmp_reader->bmp_info_header.height; ++i) {
            for (size_t j = 0; j < written_width_; ++j) {
                // Расчет компоненты Y для каждого пикселя
                y_frame_[i * written_width_ + j] =
                        bmp_reader->converted_data[i][j][RGB::kR] * 0.299 +
                        0.587 * bmp_reader->converted_data[i][j][RGB::kG] +
                        0.114 * bmp_reader->converted_data[i][j][RGB::kB];
            }
        }

        for (size_t i = 0; i < bmp_reader->bmp_info_header.height; i += 2) {
            for (size_t j = 0; j < written_width_; j += 2) {
                uint8_t count = 0;
                uint16_t u_sum = UCalculation(bmp_reader->converted_data[i][j]);
                count++;
                if (j + 1 < written_width_) {
                    u_sum += UCalculation(bmp_reader->converted_data[i][j + 1]);
                    count++;
                }
                if (i + 1 < bmp_reader->bmp_info_header.height) {
                    u_sum += UCalculation(bmp_reader->converted_data[i + 1][j]);
                    count++;
                }
                if (count == 3) {
                    u_sum += UCalculation(bmp_reader->converted_data[i + 1][j + 1]);
                    count++;
                }
                // Запись компоненты U
                u_frame_.push_back(u_sum / count);

                count = 0;
                uint16_t v_sum = VCalculation(bmp_reader->converted_data[i][j]);
                count++;
                if (j + 1 < written_width_) {
                    v_sum += VCalculation(bmp_reader->converted_data[i][j + 1]);
                    count++;
                }
                if (i + 1 < bmp_reader->bmp_info_header.height) {
                    v_sum += VCalculation(bmp_reader->converted_data[i + 1][j]);
                    count++;
                }
                if (count == 3) {
                    v_sum += VCalculation(bmp_reader->converted_data[i + 1][j + 1]);
                    count++;
                }
                // Запись компоненты V
                v_frame_.push_back(v_sum / count);
            }
        }
    }
}