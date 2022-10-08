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
    float distance_between_major_ticks;
    ImVec2 canvas_tl_corner;
    ImVec2 canvas_size;
    ImVec2 canvas_br_corner;
    ImDrawList* drawList;
    float tick_width = 2;
    float second_tick_length = 15;
    ImU32 tick_color = IM_COL32(0, 255, 0, 255);
    ImU32 ruler_body_color = IM_COL32(255, 0, 0, 255);
    float ruler_height = 20;
    // the seconds between major ticks is obtained by raising 2 to this power
    int ruler_zoom = -1;
    int ruler_zoom_min = -1;
    int ruler_zoom_max = 6;
    const float sidebar_width = 20.f;
    bool is_playing = false;

    void draw_ruler(eklib::Scene& scene);
    void draw_sidebar(eklib::Scene& scene);
};

}
