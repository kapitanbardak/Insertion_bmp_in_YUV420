#include "bmp_reader.h"

BMPReader::BMPReader(const char *fname) {
    Read(fname);
}

void BMPReader::Read(const char *fname) {
    std::ifstream inp{fname, std::ios_base::binary};

    if (inp) {
        inp.read((char *) &file_header, sizeof(file_header));
        if (file_header.file_type != 0x4D42) {
            throw std::runtime_error("Error! Unrecognized file format.");
        }

        inp.read((char *) &bmp_info_header, sizeof(bmp_info_header));
        // The BMPColorHeader is used only for transparent images 24
        if (bmp_info_header.bit_count == 32) {
            throw std::runtime_error("Error! Unrecognized file format.");
        }

        converted_data.resize(bmp_info_header.height);
        for (int i = 0; i < bmp_info_header.height; i++) {
            converted_data[i].resize(bmp_info_header.width);
        }

        // Jump to the pixel data location
        inp.seekg(file_header.offset_data, inp.beg);
        bmp_info_header.size = sizeof(BMPInfoHeader);
        file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        file_header.file_size = file_header.offset_data;
        if (bmp_info_header.height < 0) {
            throw std::runtime_error(
                    "The program can treat only BMPReader images with the origin in the bottom left corner!");
        }
        data.resize(
                (size_t) bmp_info_header.width * (size_t) bmp_info_header.height * (size_t) bmp_info_header.bit_count /
                (size_t) 8);
        // Here we check if we need to take into account row padding
        if (bmp_info_header.width % 4 == 0) {
            inp.read((char *) data.data(), data.size());
            file_header.file_size += data.size();
        } else {
            uint32_t row_stride = bmp_info_header.width * bmp_info_header.bit_count / 8;
            uint32_t new_stride = MakeStrideAligned(4);
            std::vector<uint8_t> padding_row(new_stride - row_stride);

            for (int y = 0; y < bmp_info_header.height; ++y) {
                inp.read((char *) (data.data() + row_stride * y), row_stride);
                inp.read((char *) padding_row.data(), padding_row.size());
            }
            file_header.file_size += data.size() + bmp_info_header.height * padding_row.size();
        }
    } else {
        throw std::runtime_error("Unable to open the input image file.");
    }

    SetPixelsMatrix();
}

// Add 1 to the row_stride until it is divisible with align_stride
uint32_t BMPReader::MakeStrideAligned(uint32_t align_stride) {
    uint32_t new_stride = row_stride;
    while (new_stride % align_stride != 0) {
        new_stride++;
    }
    return new_stride;
}

void BMPReader::SetPixelsMatrix() {
    for (size_t i = 0; i < data.size(); i+=3) {
        size_t row = bmp_info_header.height - 1 - i / 3 / bmp_info_header.width;
        size_t col = i / 3 % bmp_info_header.width;

        converted_data[row][col][0] = data[i + 2];
        converted_data[row][col][1] = data[i + 1];
        converted_data[row][col][2] = data[i];
    }
}