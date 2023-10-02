#ifndef PICTURETOVIDEO_BMP_READER_H
#define PICTURETOVIDEO_BMP_READER_H

#include <vector>
#include <fstream>
#include <iostream>
#include <thread>

#pragma pack(push, 1) // Устанавливаем выравнивание структур в 1 байт
struct BMPFileHeader {
    uint16_t file_type{0x4D42};          // Тип файла, всегда "BM" (0x4D42)
    uint32_t file_size{0};               // Размер файла (в байтах)
    uint16_t reserved1{0};               // Зарезервировано, всегда 0
    uint16_t reserved2{0};               // Зарезервировано, всегда 0
    uint32_t offset_data{0};             // Смещение до данных пикселей (в байтах с начала файла)
};

struct BMPInfoHeader {
    uint32_t size{0};                      // Размер данного заголовка (в байтах)
    int32_t width{0};                      // Ширина изображения в пикселях
    int32_t height{0};                     // Высота изображения в пикселях
    //       (если положительное значение, снизу вверх, начало в нижнем левом углу)
    //       (если отрицательное значение, сверху вниз, начало в верхнем левом углу)
    uint16_t planes{1};                    // Количество плоскостей для устройства, всегда 1
    uint16_t bit_count{0};                 // Количество бит на пиксель
    uint32_t compression{
            0};               // 0 или 3 - без сжатия. Эта программа поддерживает только несжатые изображения BMP
    uint32_t size_image{0};                // 0 - для несжатых изображений
    int32_t x_pixels_per_meter{0};         // Количество пикселей на метр по горизонтали
    int32_t y_pixels_per_meter{0};         // Количество пикселей на метр по вертикали
    uint32_t colors_used{
            0};               // Количество цветовых индексов в таблице цветов. Используйте 0 для максимального числа цветов, разрешенных bit_count
    uint32_t colors_important{
            0};               // Количество цветов, используемых для отображения изображения. Если 0, все цвета требуются.
};

#pragma pack(pop) // Восстанавливаем стандартное выравнивание

class BMPReader {
public:
    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    std::vector<std::vector<std::array<unsigned char, 3>>> converted_data; // Массив для хранения преобразованных данных

    BMPReader(const char *fname); // Конструктор класса
    void Read(const char *fname); // Метод для чтения изображения из файла

private:
    uint32_t row_stride{0};     // Ширина строки в байтах, с выравниванием
    std::vector<uint8_t> data_; // Буфер для данных изображения
    std::mutex mtx;             // Мьютекс для синхронизации доступа к данным
    uint32_t MakeStrideAligned(uint32_t align_stride); // Метод для выравнивания ширины строки
    void SetPixelsMatrix(size_t start, size_t end);     // Метод для установки данных изображения в матрицу пикселей
};

#endif //PICTURETOVIDEO_BMP_READER_H