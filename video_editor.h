#ifndef PICTURETOVIDEO_VIDEO_EDITOR_H
#define PICTURETOVIDEO_VIDEO_EDITOR_H

#include <string>
#include <fstream>
#include <iostream>
#include <array>

#include "bmp_reader.h"

namespace std {
    class VideoEditor {
    public:
        enum RGB {
            kR = 0,    // Красный
            kG = 1,  // Зеленый
            kB = 2    // Синий
        };

        void PrepareFrame(BMPReader* bmp_reader);
        void EditVideo(const char *path, size_t width, size_t height);
    private:
        vector<uint8_t> frame_data_;

        vector<uint8_t> y_frame_;
        vector<uint8_t> u_frame_;
        vector<uint8_t> v_frame_;

        size_t u_shift_;
        size_t v_shift_;

        size_t written_width_ = 0;

        void ResetUVChannels(size_t cursor, size_t dest);

        uint8_t UCalculation(array<uint8_t, 3>);
        uint8_t VCalculation(array<uint8_t, 3>);
    };
}

#endif //PICTURETOVIDEO_VIDEO_EDITOR_H
