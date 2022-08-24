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

    void update();

    /**
     * Places the animation at the end of the animation queue, i.e. immediately after this function, that animation
     * will be the last one to play.
     */
    void add_animation(std::unique_ptr<Animation> animation);

    void add_active_object(const std::shared_ptr<Triangle>& object);

    /**
     * Draws all of the active objects, according to the ordering of that underlying data structure.
     */
    void draw(Engine& engine, VkCommandBuffer commandBuffer);
private:
    std::list<std::unique_ptr<Animation>> animation_queue{std::list<std::unique_ptr<Animation>>()};
    std::forward_list<std::shared_ptr<Triangle>> active_objects{};
};

}
