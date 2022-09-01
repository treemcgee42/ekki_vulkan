//
// Created by Varun Malladi on 8/27/22.
//

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "engine.hpp"

class EkGui {
public:
    EkGui(eklib::Engine& engine);
    ~EkGui();

    ImDrawData* get_imgui_draw_data(bool *show_demo_window, bool *show_another_window, ImVec4 *clear_color);
private:
    ImGuiIO *io;
    eklib::Engine& engine;

    void setup_imgui_context();
    void setup_imgui_style();
    void setup_imgui_backend();
    void load_imgui_fonts();
};


