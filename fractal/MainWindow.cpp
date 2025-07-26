#include "MainWindow.h"

cl::Device OpenCL_init() {
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        std::cout << " No platforms found. Check OpenCL installation!\n";
        //system("pause");
        exit(1);
    }
    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0) {
        std::cout << " No devices found. Check OpenCL installation!\n";
        //system("pause");
        exit(1);
    }
    cl::Device default_device = all_devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
    return default_device;
}

void control(double& hx, double& hy, double& sizeAreaHeight, double& sizeAreaWidth,
    int& move, int& iter, int& iter_move, double& scaleArea) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        hy = (hy - sizeAreaHeight / move);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        hx = (hx - sizeAreaWidth / move);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        hy = (hy + sizeAreaHeight / move);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        hx = (hx + sizeAreaWidth / move);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && iter > 10) {
        iter -= iter_move;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        iter += iter_move;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        sizeAreaWidth /= scaleArea;
        sizeAreaHeight /= scaleArea;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        sizeAreaWidth *= scaleArea;
        sizeAreaHeight *= scaleArea;
    }
}

void imGuiWindow() {

}

std::string getKernel() {
    std::string res = "";
    char line[5000];
    std::ifstream fin("main.cl");
    while (!fin.eof()) {
        fin.getline(line, 5000);
        res += (std::string)line + "\n";
    }
    fin.close();
    return res;
}

MainWindow::MainWindow() {}

void MainWindow::run(int& width, int& height, std::string& fractal_name, std::string& texture_name) {
    // для дополнительного управления фракталами
    int addit = 2;
    double degree = 130.;
    double degree_speed = 0.2;
    const double f64_0 = 0., f64_1 = 1., f64_10 = 10., f64_360 = 360.;
    double r = 0.7885;
    //
    int i, j, t1, t2, it;
    float fps;
    int iter_move = 10;
    int move = 60;
    int iter = 100;
    // для путешествия по фракталам
    double hx = 0, hy = 0, cx = 0, cy = 0, rad = 0;
    double ww = (double)width;
    double hh = (double)height;
    double sizeAreaWidth = ww / hh * (hh / 333),
        sizeAreaHeight = hh / ww * (ww / 333),
        scaleArea = 1.1;
    char* name_fanc;
    int* w_h_iter = new int[3] {width, height, iter};
    double* hx_hy_saw_sah = new double[4] {hx, hy, sizeAreaWidth, sizeAreaHeight};
    int* matrix_res = new int[width * height];
    double* cx_cy = new double[2] {0, 0};
    int* additionally = new int[1] {addit};

    // SFML
    sf::RenderWindow win(sf::VideoMode(width, height), "0.0", sf::Style::Titlebar | sf::Style::Close);
    sf::Image res; res.create(width, height);
    sf::Texture res_t;
    sf::Sprite res_s; res_s.setPosition(0, 0);
    sf::Clock clock;
    sf::Clock deltaClock;
    sf::Image texture;
    texture.loadFromFile(texture_name);
    sf::Vector2u texture_size = texture.getSize();
    int tx_w = texture_size.x - 1,
        tx_h = texture_size.y - 1,
        tx_v = tx_w / 30;
    // ~SFML

    // InGui
    ImGui::SFML::Init(win);
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(
        "fonts/DroidSans.ttf",
        17.0f, NULL, io.Fonts->GetGlyphRangesCyrillic()
    );
    ImGui::SFML::UpdateFontTexture();
    ImGuiSliderFlags drag_flags = 0;
    drag_flags |= ImGuiSliderFlags_NoInput;

    // ~InGui

    // opencl
    std::string kernel_code = getKernel();
    cl::Device default_device = OpenCL_init();
    cl::Context context({ default_device });
    cl::Program::Sources sources;
    sources.push_back({ kernel_code.c_str(), kernel_code.length() });

    //if (fractal_num == 0) 
    //else if (fractal_num == 1) name_fanc = (char*)"julia";
    //else if (fractal_num == 2) name_fanc = (char*)"burning_ship";
    //else if (fractal_num == 3) { name_fanc = (char*)"hybrid_m_b";  }
    //else if (fractal_num == 4) { name_fanc = (char*)"tricorn"; addit = 4; }
    //else return;
    name_fanc = (char*)fractal_name.c_str();
    std::cout << fractal_name << "\n";
    if (fractal_name == "hybrid_m_b")
        addit = 9;
    else if (fractal_name == "tricorn")
        addit = 4;
    cl::Program program(context, sources);
    if (program.build({ default_device }) != CL_SUCCESS) {
        std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << std::endl;
        system("pause");
        exit(1);

    }
    cl::Buffer buffer_w_iter(context, CL_MEM_READ_ONLY, sizeof(int) * 3);
    cl::Buffer buffer_hx_hy_sa(context, CL_MEM_READ_ONLY, sizeof(double) * 4);
    cl::Buffer buffer_cx_cy(context, CL_MEM_READ_ONLY, sizeof(double) * 2);
    cl::Buffer buffer_hybrid(context, CL_MEM_READ_ONLY, sizeof(int) * 1);
    cl::Buffer buffer_matrix(context, CL_MEM_READ_WRITE, sizeof(int) * width * height);
    //cl::Buffer buffer_additional(context, CL_MEM_READ_WRITE, sizeof(int) * width * height);
    cl::CommandQueue queue(context, default_device);

    cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer> fractal(cl::Kernel(program, name_fanc));

    cl::EnqueueArgs eargs(queue, cl::NDRange(width * height));
    // ~opencl

    //delete[] name_fanc;

    while (win.isOpen()) {
        sf::Event ev;
        while (win.pollEvent(ev)) {
            ImGui::SFML::ProcessEvent(ev);
            if (ev.type == sf::Event::Closed) {
                ImGui::SFML::Shutdown();
                win.close();
                delete[] matrix_res;
                delete[] w_h_iter;
                delete[] hx_hy_saw_sah;
                exit(0);
            }
        }

        ImGui::SFML::Update(win, deltaClock.restart());
        if (ImGui::Begin("window!!!")) {
            if (ImGui::Button(u8"back")) {
                ImGui::SFML::Shutdown();
                win.close();
                delete[] matrix_res;
                delete[] w_h_iter;
                delete[] hx_hy_saw_sah;
                return;
            }
            ImGui::Text("x = %.15f", hx); ImGui::SameLine(); if (ImGui::Button(u8"defult x")) hx = 0;
            ImGui::Text("y = %.15f", hy); ImGui::SameLine(); if (ImGui::Button(u8"defult y")) hy = 0;
            ImGui::Text("zoom1 = %.15f", sizeAreaWidth); ImGui::SameLine();
            if (ImGui::Button(u8"defult zoom1")) {
                sizeAreaWidth = ww / hh * (hh / 333);
                sizeAreaHeight = hh / ww * (ww / 333);
            }
            ImGui::Text("zoom2 = %.15f", sizeAreaHeight);
            ImGui::Text("quality = %i", iter); ImGui::SameLine(); if (ImGui::Button(u8"defult quality")) iter = 100;
            if (ImGui::Button("-1000")) if (iter > 1010) iter -= 1000; ImGui::SameLine();
            if (ImGui::Button("-100")) if (iter > 110) iter -= 100; ImGui::SameLine();
            if (ImGui::Button("-10")) if (iter > 19) iter -= 10; ImGui::SameLine();
            if (ImGui::Button("-1")) if (iter > 11) iter -= 1; ImGui::SameLine();
            if (ImGui::Button("+1")) iter += 1; ImGui::SameLine();
            if (ImGui::Button("+10")) iter += 10; ImGui::SameLine();
            if (ImGui::Button("+100")) iter += 100; ImGui::SameLine();
            if (ImGui::Button("+1000")) iter += 1000;



            if (fractal_name == "julia") {
                ImGui::Separator();
                ImGui::Text(u8"r * cos( rad ) + i * r * sin( rad)");
                ImGui::Text("rad = %.15f", rad); ImGui::SameLine();
                ImGui::Text("r = %.4f", r);
                ImGui::DragScalar(u8"r", ImGuiDataType_Double, &r, 0.0001, &f64_0, &f64_1, "%.4f", drag_flags);
                ImGui::SameLine(); if (ImGui::Button(u8"defult r")) r = 0.7885;
                ImGui::DragScalar(u8"degree", ImGuiDataType_Double, &degree, degree_speed, &f64_0, &f64_360, "%.15F", drag_flags);
                ImGui::SameLine(); if (ImGui::Button(u8"defult degree")) degree = 130.;
                ImGui::SliderScalar(u8"degree_speed", ImGuiDataType_Double, &degree_speed, &f64_0, &f64_10, "%.15F", drag_flags | ImGuiSliderFlags_Logarithmic);
                ImGui::SameLine(); if (ImGui::Button(u8"defult degree_speed")) degree_speed = 0.2;
                rad = degree / 360. * (2.0 * 3.14159265358979);
                cx = r * cos(rad);
                cy = r * sin(rad);
                cx_cy[0] = cx;
                cx_cy[1] = cy;
                queue.enqueueWriteBuffer(buffer_cx_cy, CL_TRUE, 0, sizeof(double) * 2, cx_cy);
            }
            else if (fractal_name == "hybrid") {
                ImGui::Separator();
                ImGui::SliderInt("hybrid(b~m)", &addit, 0, 10, NULL, drag_flags);
                additionally[0] = addit - 1;
                queue.enqueueWriteBuffer(buffer_hybrid, CL_TRUE, 0, sizeof(int) * 1, additionally);
            }
            else if (fractal_name == "tricorn") {
                ImGui::Separator();
                ImGui::SliderInt("hybrid(b~m)", &addit, 2, 6, NULL, drag_flags);
                additionally[0] = addit - 1;
                queue.enqueueWriteBuffer(buffer_hybrid, CL_TRUE, 0, sizeof(int) * 1, additionally);
            }
            ImGui::End();
        }

        control(hx, hy, sizeAreaHeight, sizeAreaWidth, move, iter, iter_move, scaleArea);

        // get img from GPU
        w_h_iter[0] = width;
        w_h_iter[1] = height;
        w_h_iter[2] = iter;
        hx_hy_saw_sah[0] = hx;
        hx_hy_saw_sah[1] = hy;
        hx_hy_saw_sah[2] = sizeAreaWidth;
        hx_hy_saw_sah[3] = sizeAreaHeight;
        queue.enqueueWriteBuffer(buffer_w_iter, CL_TRUE, 0, sizeof(int) * 3, w_h_iter);
        queue.enqueueWriteBuffer(buffer_hx_hy_sa, CL_TRUE, 0, sizeof(double) * 4, hx_hy_saw_sah);
        fractal(eargs, buffer_w_iter, buffer_hx_hy_sa, buffer_cx_cy, buffer_hybrid, buffer_matrix).wait();
        queue.enqueueReadBuffer(buffer_matrix, CL_TRUE, 0, sizeof(int) * width * height, matrix_res);
        // ~get img from GPU

        // draw img
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {

                it = matrix_res[i * width + j];
                if (it == 0) {
                    res.setPixel(j, i, sf::Color(0, 0, 0));
                }
                else {

                    t1 = it / 100;
                    t2 = (tx_h / 100.0) * (it % 100);
                    res.setPixel(j, i, texture.getPixel(t1, t2));
                }
            }
        }
        res_t.loadFromImage(res);
        res_s.setTexture(res_t);
        win.draw(res_s);
        // ~draw img

        // fps
        fps = 1.f / clock.getElapsedTime().asSeconds();
        win.setTitle(std::to_string(fps));
        clock.restart();
        // ~fps

        // ImGui render
        ImGui::SFML::Render(win);
        // ~ImGui render

        win.display();
    }
    return;
}