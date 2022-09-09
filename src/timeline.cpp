//
// Created by Varun Malladi on 9/8/22.
//

#include "src/include/timeline.hpp"

#include <iostream>

namespace eklib {

Timeline::Timeline(float scene_duration) {
    auto length = static_cast<int>(std::floor(scene_duration));
    animations.resize(length+1);

    for (int i=0; i<animations.size(); i++) {
        animations[i] = std::deque<std::shared_ptr<Animation>>();
    }
}

void Timeline::insert(std::shared_ptr<Animation> &animation) {
    auto object_id = animation->get_object_id();
    bool no_object_animations = (object_animations_map.count(object_id) == 0);

    // connect beginning and ending states
    if (!no_object_animations) {
        for (auto& a : object_animations_map[object_id]) {
            if (a->get_end_time() == animation->get_start_time()) {
                animation->change_initial_state(a->get_final_state());
            }

            if (a->get_start_time() == animation->get_end_time()) {
                a->change_initial_state(animation->get_final_state());
            }
        }
    }

    // insert into timeline
    auto lower_floor = static_cast<int>(std::floor(animation->get_start_time()));
    auto upper_flower = static_cast<int>(std::floor(animation->get_end_time()));

    for (int i=lower_floor; i<=upper_flower; i++) {
        animations[i].push_back(animation);
    }

    // update object animation map
    if (no_object_animations) {
        object_animations_map[object_id] = std::deque<std::shared_ptr<Animation>>();
    }
    object_animations_map[object_id].push_back(animation);
}

std::deque<std::shared_ptr<Animation>> Timeline::get_active_animations(float time) {
    auto to_return = std::deque<std::shared_ptr<Animation>>();

    auto floor = static_cast<int>(std::floor(time));
    for (const auto& animation : animations[floor]) {
        if (animation->get_start_time() <= time && animation->get_end_time() >= time) {
            to_return.push_back(animation);
        }
    }

    return std::move(to_return);
}

}
