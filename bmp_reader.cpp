#include "bmp_reader.h"

// Конструктор класса BMPReader
BMPReader::BMPReader(const char *fname) {
    Read(fname);
}

// Метод для чтения изображения из файла
void BMPReader::Read(const char *fname) {
    std::ifstream inp{fname, std::ios_base::binary};

    if (inp) {
        inp.read((char *) &file_header, sizeof(file_header));
        if (file_header.file_type != 0x4D42) {
            throw std::runtime_error("Ошибка! Нераспознанный формат файла.");
        }

        inp.read((char *) &bmp_info_header, sizeof(bmp_info_header));
        // BMPColorHeader используется только для изображений с прозрачностью (24 бита)
        if (bmp_info_header.bit_count == 32) {
            throw std::runtime_error("Ошибка! Нераспознанный формат файла.");
        }

        converted_data.resize(bmp_info_header.height);
        for (int i = 0; i < bmp_info_header.height; i++) {
            converted_data[i].resize(bmp_info_header.width);
        }

        // Переход к данным пикселей
        inp.seekg(file_header.offset_data, inp.beg);
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        file_header.file_size = file_header.offset_data;
        if (bmp_info_header.height < 0) {
            throw std::runtime_error(
                    "Программа поддерживает только изображения BMPReader с началом в нижнем левом углу!");
        }
        data_.resize(
                (size_t) bmp_info_header.width * (size_t) bmp_info_header.height * (size_t) bmp_info_header.bit_count /
                (size_t) 8);
        // Здесь проверяем, нужно ли учитывать заполнение строк
        if (bmp_info_header.width % 4 == 0) {
            inp.read((char *) data_.data(), data_.size());
            file_header.file_size += data_.size();
        } else {
            uint32_t row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            uint32_t new_stride = MakeStrideAligned(4);
            std::vector<uint8_t> padding_row(new_stride - row_stride);

            for (int y = 0; y < bmp_info_header.height; ++y) {
                inp.read((char *) (data_.data() + row_stride * y), row_stride);
                inp.read((char *) padding_row.data(), padding_row.size());
            }
            file_header.file_size += data_.size() + bmp_info_header.height * padding_row.size();
        }
    } else {
        throw std::runtime_error("Не удалось открыть файл изображения.");
    }

    // Задаем параметры для первого потока
    size_t start1 = 0;
    size_t end1 = data_.size() / 2; // Половина данных

    // Задаем параметры для второго потока
    size_t start2 = end1;
    size_t end2 = data_.size();

    // Создаем два потока
    std::thread thread1(&BMPReader::SetPixelsMatrix, this, start1, end1);
    std::thread thread2(&BMPReader::SetPixelsMatrix, this, start2, end2);

    // Ждем завершения работы потоков
    thread1.join();
    thread2.join();
}

// Добавляет 1 к row_stride до тех пор, пока он не будет делиться на align_stride
uint32_t BMPReader::MakeStrideAligned(uint32_t align_stride) {
    uint32_t new_stride = row_stride;
    while (new_stride % align_stride != 0) {
        new_stride++;
    }
    return new_stride;
}

// Устанавливает данные пикселей в матрицу пикселей
void BMPReader::SetPixelsMatrix(size_t start, size_t end) {
    for (size_t i = start; i < end; i += 3) {
        size_t row = bmp_info_header.height - 1 - i / 3 / bmp_info_header.width;
        size_t col = i / 3 % bmp_info_header.width;

        mtx.lock(); // Захватываем мьютекс перед доступом к данным
        converted_data[row][col][0] = data_[i + 2]; // Красная компонента
        converted_data[row][col][1] = data_[i + 1]; // Зеленая компонента
        converted_data[row][col][2] = data_[i];     // Синяя компонента
        mtx.unlock(); // Освобождаем мьютекс после доступа к данным
    }
}