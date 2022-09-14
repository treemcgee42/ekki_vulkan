//
// Created by Varun Malladi on 9/9/22.
//

#include "src/gui/include/timeline.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include <iostream>

namespace ekgui {

Timeline::Timeline(TimelineCreateInfo& create_info):
    distance_between_seconds{create_info.distance_between_seconds}
{}

void Timeline::draw(eklib::Scene& scene) {
    ImGui::Begin("timeline");


    drawList = ImGui::GetWindowDrawList();
    canvas_tl_corner = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
    canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available
    const ImU32 canvasColor = IM_COL32(32, 32, 32, 255);


    drawList->AddRectFilled(canvas_tl_corner, canvas_tl_corner + canvas_size, canvasColor);
    ImGui::BeginChild("timeline", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    ImGui::InvisibleButton("dummy", {(1 + scene.get_duration()) * distance_between_seconds, 1});

//    float scene_time = scene.get_current_time();
//    ImGui::InputFloat("scene time", &scene_time, 0.25f);
//    scene.adjust_current_time(scene_time);

    // draw ruler
    draw_ruler(scene);




    ImGui::EndChild();
    ImGui::End();
}

void Timeline::draw_ruler(eklib::Scene& scene) {
    auto distance_between_ticks = distance_between_seconds / 4;

    // body of the ruler
    auto ruler_start_position = canvas_tl_corner;
    auto ruler_end_position = ruler_start_position + ImVec2(canvas_size.x, 20);
    drawList->AddRectFilled(ruler_start_position, ruler_end_position, IM_COL32(255, 0, 0, 255));

    // find the first visible tick
    // ScrollX() will tell us what pixel of the canvas is the at the beginning. There is a
    // second tick at every `distance_between_ticks` pixels.
    auto scroll_x = ImGui::GetScrollX();
    auto scroll_x_tick = scroll_x / distance_between_ticks;
    auto first_tick = std::ceil(scroll_x_tick);
    auto first_tick_x = first_tick * distance_between_ticks - scroll_x;
    int tick_type = static_cast<int>(first_tick) % 4;

    // determine how many ticks we need to draw (currently assuming quarter second subdivisions)
    auto number_of_ticks = std::floor((ruler_end_position.x - (ruler_start_position.x + tick_width + first_tick_x)) / distance_between_ticks) + 1;

    // draw the ticks
    float i = 0;
    float tick_time = static_cast<int>(first_tick) * 0.25;
    while (i < number_of_ticks) {
        // seeing if this is a second, half-second, or quarter-second
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

        auto tick_start_position = ImVec2(ruler_start_position.x + first_tick_x + i * distance_between_ticks, ruler_start_position.y);
        auto tick_end_position = tick_start_position + ImVec2(tick_width, this_tick_length);

        drawList->AddRectFilled(tick_start_position, tick_end_position, tick_color);

        if (tick_type == 0) {
            char time_string[10];
            sprintf(time_string, "%d", static_cast<int>(tick_time));
            drawList->AddText(tick_start_position + ImVec2(0.5f*distance_between_ticks, 0.5f*(tick_end_position.y - tick_start_position.y)), tick_color, time_string);

        }

        i += 1;
        tick_time += 0.25;
        tick_type = (tick_type + 1) % 4;
    }

    // draw current time marker
    // 1. check if that time is currently visible on the timeline. only continue if it is.
    // 2. determine the time the first tick represents, and its offset
    // 3. draw at (ticks between first tick and current time) * distance between ticks + first tick offset
    auto ct = scene.get_current_time();
    float current_time_tick = ct * 4;
    auto ct_tick_p1 = ruler_start_position.x + (current_time_tick - scroll_x_tick) * distance_between_ticks;

    auto ct_tick_start = ImVec2(ct_tick_p1, ruler_start_position.y);
    auto ct_tick_end = ImVec2(ct_tick_p1 + tick_width, (canvas_tl_corner + canvas_size).y);

    drawList->AddRectFilled(
        ct_tick_start,
        ct_tick_end,
        IM_COL32(0, 0, 255, 255)
    );

    // adjust scene time based on a click
    if (ImGui::IsMouseClicked(0)) {


        // get time of where clicked and adjust scene time
        auto mouse_x = ImGui::GetMousePos().x - canvas_tl_corner.x;
        auto time = ((mouse_x - first_tick_x) / distance_between_ticks) * 0.25 + first_tick / 4;
        //std::cout << "clicked at " << mouse_x << ", time: " << time << "\n";
        scene.adjust_current_time(time);
    }
}

}
