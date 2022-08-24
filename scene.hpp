//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include <forward_list>

#include "animation.hpp"
#include "triangle.hpp"
#include "engine.hpp"

namespace eklib {

class Scene {
public:
    ~Scene() = default;

    bool update();
    void draw(Engine& engine, VkCommandBuffer commandBuffer);

    Animation animation{0};
    std::forward_list<std::shared_ptr<Triangle>> active_objects{};

private:

};

}
