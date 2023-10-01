#ifndef PICTURETOVIDEO_BMP_READER_H
#define PICTURETOVIDEO_BMP_READER_H

#include <vector>
#include <fstream>
#include <iostream>

#pragma pack(push,1)
struct BMPFileHeader {
    uint16_t file_type{0x4D42};          // File type always BM which is 0x4D42
    uint32_t file_size{0};               // Size of the file (in bytes)
    uint16_t reserved1{0};               // Reserved, always 0
    uint16_t reserved2{0};               // Reserved, always 0
    uint32_t offset_data{0};             // Start position of pixel data (bytes from the beginning of the file)
};

struct BMPInfoHeader {
    uint32_t size{0};                      // Size of this header (in bytes)
    int32_t width{0};                      // width of bitmap in pixels
    int32_t height{0};                     // width of bitmap in pixels
    //       (if positive, bottom-up, with origin in lower left corner)
    //       (if negative, top-down, with origin in upper left corner)
    uint16_t planes{1};                    // No. of planes for the target device, this is always 1
    uint16_t bit_count{0};                 // No. of bits per pixel
    uint32_t compression{0};               // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMPReader images
    uint32_t size_image{0};                // 0 - for uncompressed images
    int32_t x_pixels_per_meter{0};
    int32_t y_pixels_per_meter{0};
    uint32_t colors_used{0};               // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
    uint32_t colors_important{0};          // No. of colors used for displaying the bitmap. If 0 all colors are required
};

#pragma pack(pop)

class BMPReader {
public:
    BMPFileHeader file_header;
    BMPInfoHeader bmp_info_header;
    std::vector<std::vector<std::array<unsigned char, 3>>> converted_data;
    BMPReader(const char *fname);
    void Read(const char *fname);


private:
    uint32_t row_stride{0};
    std::vector<uint8_t> data;
    uint32_t MakeStrideAligned(uint32_t align_stride);
    void SetPixelsMatrix();
};

#endif //PICTURETOVIDEO_BMP_READER_H