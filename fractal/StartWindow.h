#pragma once
#include "main.h"

class StartWindow {
private:

    class resolution {
    public:
        int w;
        int h;
        bool square;
        resolution(int w, int h, bool square);
    };
    std::vector<resolution> defult_resolution;

    int w, h;
    int width, height;

    sf::Event ev;

    sf::RenderWindow initWin;
    sf::Clock deltaClock;

    ImGuiWindowFlags window_flags;
    ImGuiSliderFlags drag_flags;

    bool square;
    int radioBtn;
    int exit_code;

    std::vector<std::string> vec_fractals;
    char** fractals;
    int size_fractals;
    int fractal_num;
    std::string fractal_name;

    std::string path2img;
    std::vector<std::string> vec_textures;
    char** textures;
    int size_textures;
    int texture_num;
    std::string texture_name;

    void shutdown(std::string error);

    void load_font();
    
    void get_fractals_names();
    void get_textures_names();

    void imgui_management();
    void imgui_resolution();
    void imgui_fractal();
    void imgui_texture();
    void imgui_btnGo();
    void imgui();

public:
    StartWindow();
    ~StartWindow();
    void get_data(int& width, int& height,
        std::string& fractal_name, std::string& texture_name);
    int run();
};