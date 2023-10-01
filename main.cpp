#include "bmp_reader.h"
#include "video_editor.h"

using namespace std;

int main() {
    // Read an image from disk, modify it and write it back:
    BMPReader *bmp = new BMPReader(
            "/Users/aleksejrusskih/Nextcloud/Программирование/Тестовые задания/PictureIntoVideo/picture.bmp");
        VideoEditor *video_editor = new VideoEditor();
        video_editor->PrepareFrame(bmp);
//    for (size_t i = 0; i < bmp->converted_data.size(); ++i) {
//        for (size_t j = 0; j < bmp->converted_data[0].size(); ++j) {
//            if (video_editor->y_frame_[i * bmp->converted_data[0].size() + j] > 50)
//                cout << 0;
//            else
//                cout << ".";
//            cout << " ";
//        }
//        cout << endl;
//    }
        video_editor->EditVideo("/Users/aleksejrusskih/Nextcloud/Программирование/Тестовые задания/PictureIntoVideo/bus.yuv", 352, 288);
    return 0;
}