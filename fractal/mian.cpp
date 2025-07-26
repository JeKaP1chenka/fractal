#include "main.h"
#include "StartWindow.h"
#include "MainWindow.h"


int main() {
    setlocale(LC_ALL, "rus");
    int width = 1000;
    int height = 1000;
    int fractal_num = 0;
    std::string fractal_name;
    std::string texture_name;

    StartWindow strWin;
    int t = 1;
    while (t) {
        t = strWin.run();
        std::cout << 123 << "\n";
        //Sleep(1000);
        strWin.get_data(width, height, fractal_name, texture_name);
        MainWindow win;
        win.run(width, height, fractal_name, texture_name);
    }

	return 0;
}