//
// Created by Varun Malladi on 9/9/22.
//

#pragma once

#include "src/include/scene.hpp"

namespace ekgui {

struct TimelineCreateInfo {
    float time_at_beginning;
    float time_at_end;
    float distance_between_seconds;
};

class Timeline {
public:
    Timeline(TimelineCreateInfo& create_info);

    void draw(eklib::Scene& scene);

private:
    float time_at_beginning;
    float time_at_end;
    float distance_between_seconds;
    ImVec2 canvas_tl_corner;
    ImVec2 canvas_size;
    ImDrawList* drawList;
    float tick_width = 2;
    float second_tick_length = 15;
    ImU32 tick_color = IM_COL32(0, 255, 0, 255);

    void draw_ruler(eklib::Scene& scene);
};

}
