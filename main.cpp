#include "bmp_reader.h"
#include "video_editor.h"

using namespace std;

int main() {
    // Чтение изображения с диска, его модификация и запись в видео:

    // Создаем объект BMPReader на куче и загружаем изображение
    BMPReader *bmp = new BMPReader("../picture.bmp");

    // Создаем объект VideoEditor на куче
    VideoEditor *video_editor = new VideoEditor();

    // Подготавливаем кадр из изображения
    video_editor->PrepareFrame(bmp);

    // Редактируем видео и сохраняем результат
    video_editor->EditVideo("../bus.yuv", 352, 288);

    // Освобождаем память, удаляя объекты с кучи
    delete bmp;
    delete video_editor;

    return 0;
}
