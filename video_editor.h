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
            kR = 0,    // Компонента красного цвета
            kG = 1,    // Компонента зеленого цвета
            kB = 2     // Компонента синего цвета
        };

        // Подготавливает кадр из объекта BMPReader и сохраняет его в формате YUV420.
        void PrepareFrame(BMPReader *bmp_reader);

        // Редактирует видео в формате YUV420 и сохраняет результат в новый файл.
        void EditVideo(const char *path, size_t width, size_t height);

    private:
        vector<uint8_t> frame_data_;  // Буфер для данных кадра

        vector<uint8_t> y_frame_;     // Компонента Y
        vector<uint8_t> u_frame_;     // Компонента U
        vector<uint8_t> v_frame_;     // Компонента V

        size_t u_shift_;              // Сдвиг для компоненты U
        size_t v_shift_;              // Сдвиг для компоненты V

        size_t written_width_ = 0;    // Ширина записываемого изображения

        // Рассчитывает компоненту U для заданного пикселя в формате YUV420.
        uint8_t UCalculation(array<uint8_t, 3> pixel);

        // Рассчитывает компоненту V для заданного пикселя в формате YUV420.
        uint8_t VCalculation(array<uint8_t, 3> pixel);
    };
}

#endif //PICTURETOVIDEO_VIDEO_EDITOR_H