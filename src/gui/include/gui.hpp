//
// Created by Varun Malladi on 8/27/22.
//

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "src/include/engine.hpp"
#include "src/include/scene.hpp"
#include "src/include/triangle.hpp"
#include "src/gui/include/timeline.hpp"

class EkGui {
public:
    EkGui(eklib::Engine& engine);
    ~EkGui();

    ImDrawData* get_imgui_draw_data(bool *show_demo_window, bool *show_another_window, ImVec4 *clear_color, eklib::Scene& scene);
private:
    ImGuiIO *io;
    eklib::Engine& engine;
    int selected_object_index_in_active_objects = -1;
    std::unique_ptr<ekgui::Timeline> timeline;

    float obj_editor_scale = 1.0;
    float obj_editor_position2[2];
    float obj_editor_color4[4];

    void setup_imgui_context();
    void setup_imgui_style();
    void setup_imgui_backend();
    void load_imgui_fonts();

    void create_object_list_window(eklib::Scene& scene);
    void create_object_editor_window(eklib::Scene& scene);
    void create_timeline_window(eklib::Scene& scene);
};


