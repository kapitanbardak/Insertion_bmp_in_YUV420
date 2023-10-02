#include "bmp_reader.h"
#include "video_editor.h"

using namespace std;

int main() {
    // Чтение изображения с диска, его модификация и запись в видео:

    // Выводим сообщение и читаем путь к файлу BMP
    cout << "Введите путь к файлу BMP: ";
    string bmp_file_path;
    cin >> bmp_file_path;

    // Создаем объект BMPReader на куче и загружаем изображение
    BMPReader *bmp = new BMPReader(bmp_file_path.c_str());

    // Выводим сообщение и читаем путь к файлу с видео
    cout << "Введите путь к файлу с видео: ";
    string video_file_path;
    cin >> video_file_path;

    // Выводим сообщение и читаем разрешение видео в пикселях с проверкой
    int video_width, video_height;
    do {
        std::cout << "Введите разрешение видео (ширина высота): ";
        if (!(std::cin >> video_width >> video_height) || video_width <= 0 || video_height <= 0) {
            std::cout << "Неправильный ввод. Пожалуйста, введите положительные целые числа." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } while (video_width <= 0 || video_height <= 0);

    // Выводим сообщение и читаем имя для сохранения отредактированного файла
    cout << "Введите путь и имя (path/name) для сохранения отредактированного файла: ";
    string edited_video_name;
    cin >> edited_video_name;

    // Создаем объект VideoEditor на куче
    VideoEditor *video_editor = new VideoEditor();

    // Подготавливаем кадр из изображения
    video_editor->PrepareFrame(bmp);

    // Редактируем видео и сохраняем результат
    video_editor->EditVideo(video_file_path.c_str(), video_width, video_height, edited_video_name.c_str());

    // Освобождаем память, удаляя объекты с кучи
    delete bmp;
    delete video_editor;

    return 0;
}
