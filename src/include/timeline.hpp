//
// Created by Varun Malladi on 9/8/22.
//

#pragma once

#include <vector>
#include <deque>
#include <map>
#include "animation.hpp"

namespace eklib {

class Timeline {
public:
    Timeline(float scene_duration);

    /**
     * Accordingly updates the internal animations vector.
     */
    void insert(std::shared_ptr<Animation>& animation);
    std::deque<std::shared_ptr<Animation>> get_active_animations(float time);

private:
    std::vector<std::deque<std::shared_ptr<Animation>>> animations;
    std::map<uint32_t, std::deque<std::shared_ptr<Animation>>> object_animations_map;
};

}
