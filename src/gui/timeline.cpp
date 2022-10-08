//
// Created by Varun Malladi on 9/9/22.
//

#include "src/gui/include/timeline.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <iostream>

namespace ekgui {

Timeline::Timeline(TimelineCreateInfo& create_info):
    distance_between_major_ticks{create_info.distance_between_seconds}
{}

void Timeline::draw(eklib::Scene& scene) {
    ImGui::Begin("timeline");


    drawList = ImGui::GetWindowDrawList();
    canvas_tl_corner = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
    canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
    canvas_br_corner = canvas_tl_corner + canvas_size;
    const ImU32 canvasColor = IM_COL32(32, 32, 32, 255);

    drawList->AddRectFilled(canvas_tl_corner, canvas_tl_corner + canvas_size, canvasColor);
    ImGui::BeginChild("timeline", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    ImGui::InvisibleButton("dummy", {(1 + scene.get_duration()) * distance_between_major_ticks * static_cast<float>(pow(2, -1 * ruler_zoom)), 1});

//    float scene_time = scene.get_current_time();
//    ImGui::InputFloat("scene time", &scene_time, 0.25f);
//    scene.adjust_current_time(scene_time);

    // draw ruler
    draw_ruler(scene);
    draw_sidebar(scene);



    ImGui::EndChild();
    ImGui::End();
}

void Timeline::draw_ruler(eklib::Scene& scene) {
    // ===== Determine major tick scale
    const auto zoom = std::min(std::max(ruler_zoom, ruler_zoom_min), ruler_zoom_max);
    // lossless cast since working with powers of 2
    const auto seconds_between_major_ticks = static_cast<float>(pow(2, zoom));
    const auto seconds_per_tick = seconds_between_major_ticks / 4;
    const auto distance_between_ticks = distance_between_major_ticks / 4;

    // ===== Body of the ruler
    const auto ruler_start_position = canvas_tl_corner + ImVec2(sidebar_width, 0);
    const auto ruler_end_position = ruler_start_position + ImVec2(canvas_size.x, ruler_height);
    drawList->AddRectFilled(ruler_start_position, ruler_end_position, ruler_body_color);

    // ===== Ruler ticks
    const auto scroll_x = ImGui::GetScrollX();
    const auto scroll_x_tick = scroll_x / distance_between_ticks;
    const auto first_tick = std::ceil(scroll_x_tick);
    const auto first_tick_x = first_tick * distance_between_ticks - scroll_x;

    const auto number_of_ticks = std::floor((ruler_end_position.x - (ruler_start_position.x + tick_width + first_tick_x)) / distance_between_ticks) + 1;

    // Regardless of zoom level, there are 4 divisions between major ticks.
    // 0 - major tick
    // 1/3 - small tick
    // 2 - medium tick
    // This mod 4 representation correctly describes the order in which these ticks are displayed.
    int tick_type = static_cast<int>(first_tick) % 4; // safe to cast since this is a ceiling value
    float i = 0;
    float tick_time = first_tick * seconds_per_tick;
    while (i < number_of_ticks) {
        float this_tick_length = second_tick_length;
        switch (tick_type) {
            case 0:
                break;
            case 1:
            case 3:
                this_tick_length = this_tick_length * 0.5f;
                break;
            case 2:
                this_tick_length = this_tick_length * 0.75f;
                break;
            default:
                assert(false);
        };

        const auto tick_start_position = ImVec2(ruler_start_position.x + first_tick_x + i * distance_between_ticks, ruler_start_position.y);
        const auto tick_end_position = tick_start_position + ImVec2(tick_width, this_tick_length);
        drawList->AddRectFilled(tick_start_position, tick_end_position, tick_color);

        if (tick_type == 0) {
            char time_string[10];
            sprintf(time_string, "%g", tick_time);
            drawList->AddText(tick_start_position + ImVec2(0.5f*distance_between_ticks, 0.5f*(tick_end_position.y - tick_start_position.y)), tick_color, time_string);
        }

        i += 1;
        tick_time += seconds_per_tick;
        tick_type = (tick_type + 1) % 4;
    }

    // ===== Draw current time marker
    const auto ct = scene.get_current_time();
    const float current_time_tick = ct / seconds_per_tick;

    const auto ct_tick_p1 = ruler_start_position.x + (current_time_tick - scroll_x_tick) * distance_between_ticks;
    const auto ct_tick_start = ImVec2(ct_tick_p1, ruler_start_position.y);
    const auto ct_tick_end = ImVec2(ct_tick_p1 + tick_width, (canvas_tl_corner + canvas_size).y);

    drawList->AddRectFilled(
        ct_tick_start,
        ct_tick_end,
        IM_COL32(0, 0, 255, 255)
    );

    // adjust based on a left click
    const auto mouse_pos = ImGui::GetMousePos();
    if (ImGui::IsMouseClicked(0)) {
        if (mouse_pos.x > canvas_tl_corner.x + sidebar_width && mouse_pos.x < canvas_br_corner.x && mouse_pos.y > canvas_tl_corner.y && mouse_pos.y < canvas_br_corner.y - 20) {
            const auto mouse_x = ImGui::GetMousePos().x - canvas_tl_corner.x;
            const auto time = ((mouse_x - first_tick_x) / distance_between_ticks) * seconds_per_tick +
                              first_tick * seconds_per_tick;
            scene.adjust_current_time(time);
        }
    }
}

void Timeline::draw_sidebar(eklib::Scene& scene) {
    const float icon_padding = 5.f;
    const auto mouse_pos = ImGui::GetMousePos();

    // ===== Sidebar outline
    const auto sidebar_tl_corner = canvas_tl_corner;
    const auto sidebar_br_corner = ImVec2(canvas_tl_corner.x + sidebar_width, canvas_br_corner.y);
    drawList->AddRectFilled(sidebar_tl_corner, sidebar_br_corner, IM_COL32(32, 32, 32, 255));

    const bool mouse_pos_within_sidebar =
            mouse_pos.x > sidebar_tl_corner.x &&
            mouse_pos.x < sidebar_br_corner.x &&
            mouse_pos.y < sidebar_br_corner.y &&
            mouse_pos.y > sidebar_tl_corner.y;
#define mouse_pos_in_button(button_index) (\
mouse_pos_within_sidebar && \
mouse_pos.y > sidebar_tl_corner.y + (button_index)*sidebar_width && \
mouse_pos.y < sidebar_tl_corner.y + ((button_index)+1)*sidebar_width)

    // ===== Play / pause button
    const int button_index = 0; // first button
    if (mouse_pos_in_button(button_index)) {
        if (ImGui::IsMouseClicked(0)) {
            is_playing = !is_playing;
            scene.is_playing = is_playing;
        }
    }

    if (!is_playing) {
        const auto play_color = ImColor(0.f, 1.f, 0.f);
        const auto play_triangle_v0 = ImVec2(sidebar_tl_corner.x + icon_padding, sidebar_tl_corner.y + icon_padding);
        const auto play_triangle_v1 = ImVec2(play_triangle_v0.x, play_triangle_v0.y + sidebar_width - 2*icon_padding);
        const auto play_triangle_v2 = ImVec2(sidebar_br_corner.x - icon_padding, sidebar_tl_corner.y + 0.5f*sidebar_width);
        drawList->AddTriangleFilled(play_triangle_v0, play_triangle_v1, play_triangle_v2, play_color);
    } else {
        const auto stop_color = ImColor(1.f, 0.f, 0.f);
        const auto stop_button_tl_corner = ImVec2(sidebar_tl_corner.x + icon_padding, sidebar_tl_corner.y + button_index*sidebar_width + icon_padding);
        const auto stop_button_br_corner = ImVec2(sidebar_br_corner.x - icon_padding, stop_button_tl_corner.y + sidebar_width - 2*icon_padding);
        drawList->AddRectFilled(stop_button_tl_corner, stop_button_br_corner, stop_color);
    }




}

}
