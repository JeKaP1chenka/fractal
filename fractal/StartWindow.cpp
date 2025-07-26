#include "StartWindow.h"

StartWindow::resolution::resolution(int w, int h, bool square) {
    this->w = w;
    this->h = h;
    this->square = square;
}

StartWindow::StartWindow() {
    this->defult_resolution = {
        resolution(500, 500, 1),
        resolution(750, 750, 1),
        resolution(1000, 1000, 1),
        resolution(1900, 1000, 0),
    };

    this->w = 1000;
    this->h = 1000;
    this->width = 500;
    this->height = 500;
    this->initWin.create(sf::VideoMode(this->width, this->height), "setings", sf::Style::Titlebar | sf::Style::Close);
    this->initWin.setFramerateLimit(24);
    this->window_flags = 0;
    this->window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    this->window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
    this->window_flags |= ImGuiWindowFlags_NoSavedSettings;
    this->drag_flags = 0;
    this->drag_flags |= ImGuiSliderFlags_NoInput;
    this->square = true;
    this->radioBtn = 2;
    this->exit_code = 0;
    this->path2img = "img\\";

    this->get_fractals_names();

    this->get_textures_names();

}

StartWindow::~StartWindow() {
        
    delete[] fractals;
    delete[] textures;
    std::cout << 123 << "\n";
}

void StartWindow::shutdown(std::string error) {
    ImGui::SFML::Shutdown();
    this->initWin.close();
    std::cout << error << "\n";
    system("pause");
    exit(1);
}

void StartWindow::load_font() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(
        "DroidSans.ttf",
        20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic()
    );
    ImGui::SFML::UpdateFontTexture();
}

void StartWindow::get_fractals_names() {
    this->vec_fractals = {
        "mandelbrot", "julia", "burning_ship", "hybrid", "tricorn" 
    };
    this->size_fractals = this->vec_fractals.size();
    this->fractals = new char* [size_fractals];
    for (int i = 0; i < this->size_fractals; i++)
        this->fractals[i] = (char*)(this->vec_fractals[i].c_str());

    this->fractal_num = 0;
}

void StartWindow::get_textures_names() {

    if (std::filesystem::is_directory(std::filesystem::status(path2img))) {

        std::string file;
        std::string extension;
        for (const auto& entry : std::filesystem::directory_iterator(this->path2img)) {
            file = entry.path().string();
            extension = "";
            for (int i = file.size()-4; i < file.size(); i++) 
                extension+=file[i];
            if (extension == ".jpg" or extension == ".png") 
                this->vec_textures.push_back(file);
        }
        if (this->vec_textures.size() == 0)
            this->shutdown("func get_textures_names: no files found");
        else {
            this->size_textures = this->vec_textures.size();
            this->textures = new char* [this->size_textures];
            for (int i = 0; i < this->size_textures; i++)
                this->textures[i] = (char*)(this->vec_textures[i].c_str());
            this->texture_num = 0;
        }
        return;
    }
    this->shutdown("func get_textures_names: folder not found");
}

void StartWindow::imgui_management() {
    ImGui::Text(u8"УПРАВЛЕНИЕ");
    ImGui::BulletText(u8"W A S D - перемещение");
    ImGui::BulletText(u8"UP/DOWN - приблизить/отдалить\n- (стрелка вверх и вниз)");
    ImGui::BulletText(u8"LEFT / RIGHT - повышение / cнижение качества\n- (стрелка влево и вправо)");

    ImGui::Separator();
}

void StartWindow::imgui_resolution() {
    ImGui::Text(u8"permission");
    ImGui::RadioButton("500x500", &this->radioBtn, 0); ImGui::SameLine();
    ImGui::RadioButton("750x750", &this->radioBtn, 1); ImGui::SameLine();
    ImGui::RadioButton("1000x1000", &this->radioBtn, 2); ImGui::SameLine();
    ImGui::RadioButton("1900x1000", &this->radioBtn, 3);

    this->square = defult_resolution[this->radioBtn].square;
    this->w = defult_resolution[this->radioBtn].w;
    this->h = defult_resolution[this->radioBtn].h;

    ImGui::Checkbox(u8"square window", &this->square);
    if (!this->square) {
        ImGui::DragInt(u8"width", &this->w, 1, 400, 1900, NULL, this->drag_flags);
        ImGui::DragInt(u8"height", &this->h, 1, 400, 1000, NULL, this->drag_flags);
    }
    else {
        if (this->w > 1000) this->w = 1000;
        this->h = this->w;
        ImGui::DragInt(u8"width", &this->w, 1, 200, 1000, NULL, this->drag_flags);
    }
    if (this->w != 500 and this->w != 750 and this->w != 1000
        and (this->w != 1900 or this->h != 1000))
        this->radioBtn = 0;
    if ((!this->square and (this->radioBtn == 0 or this->radioBtn == 1 or this->radioBtn == 2)) or
        (this->square and (this->radioBtn == 3)))
        this->radioBtn = 0;

    ImGui::Separator();
}

void StartWindow::imgui_fractal() {
    ImGui::Text(u8"fractal");
    ImGui::Combo(u8"fractal", &this->fractal_num, this->fractals, this->size_fractals);
    ImGui::Separator();
}

void StartWindow::imgui_texture() {
    ImGui::Text(u8"texture");
    ImGui::Combo(u8"texture", &this->texture_num, this->textures, this->size_textures);
    ImGui::Separator();
}

void StartWindow::imgui_btnGo() {
    if (ImGui::Button("GO")) {
        this->exit_code = 1;
        //texture_name = files[selectedIndex];
        //texture_num = selectedIndex;
    }
}

void StartWindow::imgui() {
    ImGui::SFML::Update(this->initWin, this->deltaClock.restart());
    if (ImGui::Begin("n", 0, this->window_flags)) {
        ImGui::SetWindowPos("n", ImVec2(0, 0));
        ImGui::SetWindowSize("n", ImVec2(this->width, this->height));

        this->imgui_management();
        this->imgui_resolution();
        this->imgui_fractal();
        this->imgui_texture();
        this->imgui_btnGo();

        ImGui::End();
    }
    ImGui::SFML::Render(this->initWin);
}

int StartWindow::run() {
    ImGui::SFML::Init(this->initWin, false);
    this->load_font();
    this->initWin.setVisible(true);
    while (this->initWin.isOpen()) {
        while (this->initWin.pollEvent(this->ev)) {
            ImGui::SFML::ProcessEvent(this->ev);
            if (this->ev.type == sf::Event::Closed or this->exit_code == 1) {
                if (this->exit_code == 1) {
                    this->initWin.setVisible(false);
                    this->exit_code = 0;
                    return 1;
                }
                else if (this->exit_code == 0) {
                    //ImGui::SFML::Shutdown();
                    //this->initWin.close();
                    exit(1);
                    return 0;
                }
            }
        }

        this->imgui();
        this->initWin.display();
        //this->initWin.clear();
    }
    return 0;
}

void StartWindow::get_data(int& width, int& height,
    std::string& fractal_name, std::string& texture_name) {
    width = this->w;
    height = this->h;
    fractal_name = this->fractal_name = this->vec_fractals[this->fractal_num];
    texture_name = this->texture_name = this->vec_textures[this->texture_num];
}