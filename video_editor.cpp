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
    void VideoEditor::EditVideo(const char *path, size_t width, size_t height, const char *save_path) {
        ifstream video_file(path, ios::binary);
        if (!video_file.is_open()) {
            cerr << "Ошибка открытия файла исходного видео." << endl;
        }
        std::ofstream save_file(save_path, std::ios::binary);
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

        u_frame_temp_.resize(bmp_reader->bmp_info_header.height);
        v_frame_temp_.resize(bmp_reader->bmp_info_header.height);
        for (size_t i = 0; i < u_frame_temp_.size(); ++i) {
            u_frame_temp_[i].resize(bmp_reader->bmp_info_header.width);
            v_frame_temp_[i].resize(bmp_reader->bmp_info_header.width);
        }

        written_width_ = bmp_reader->bmp_info_header.width;

        /*
         * В идеале чтобы прям совсем повыделываться можно было написать класс
         * который бы принимал аргументом число потоков определяемых пользователем при запуске
         * и пихал нагенеренные потоки в один массив, обходя который я бы здесь их и запускал
         * причём этот класс-генератор подошёл бы и для первого случая
         *
         * З.Ы. Пишу это тут просто чтобы показать, что у меня есть такая мысль в голове и я не тупой,
         * ну хотя бы не очень запись кадров выглядит ужасно :)
         */

        size_t start1 = 0;
        size_t end1 = bmp_reader->bmp_info_header.height / 2;

        size_t start2 = end1;
        size_t end2 = bmp_reader->bmp_info_header.height;

        thread thread1(&VideoEditor::ConvertationCicle, this, bmp_reader, start1, end1);
        thread thread2(&VideoEditor::ConvertationCicle, this, bmp_reader, start2, end2);

        thread1.join();
        thread2.join();

        PackingUV(bmp_reader);

    }

    void VideoEditor::ConvertationCicle(BMPReader *bmp_reader, size_t start_height, size_t end_height) {
        for (size_t i = start_height; i < end_height; ++i) {
            for (size_t j = 0; j < written_width_; ++j) {
                // Расчет компоненты Y для каждого пикселя
                y_frame_[i * written_width_ + j] =
                        bmp_reader->converted_data[i][j][RGB::kR] * 0.299 +
                        0.587 * bmp_reader->converted_data[i][j][RGB::kG] +
                        0.114 * bmp_reader->converted_data[i][j][RGB::kB];
            }
        }

        for (size_t i = start_height; i < end_height; ++i) {
            for (size_t j = 0; j < written_width_; ++j) {
                u_frame_temp_[i][j] = UCalculation(bmp_reader->converted_data[i][j]);
                v_frame_temp_[i][j] = VCalculation(bmp_reader->converted_data[i][j]);
            }
        }
    }

    void VideoEditor::PackingUV(BMPReader *bmp_reader) {
        for (size_t i = 0; i < bmp_reader->bmp_info_header.height; i += 2) {
            for (size_t j = 0; j < written_width_; j += 2) {
                uint8_t count = 1;
                uint16_t u_sum = u_frame_temp_[i][j];
                uint16_t v_sum = v_frame_temp_[i][j];
                if (j + 1 < written_width_) {
                    u_sum += u_frame_temp_[i][j + 1];
                    v_sum += v_frame_temp_[i][j + 1];
                    count++;
                }
                if (i + 1 < bmp_reader->bmp_info_header.height) {
                    u_sum += u_frame_temp_[i + 1][j];
                    v_sum += v_frame_temp_[i + 1][j];
                    count++;
                }
                if (count == 3) {
                    u_sum += u_frame_temp_[i + 1][j + 1];
                    v_sum += v_frame_temp_[i + 1][j + 1];
                    count++;
                }

                u_frame_.push_back(u_sum / count);
                v_frame_.push_back(v_sum / count);
            }
        }
    }
}