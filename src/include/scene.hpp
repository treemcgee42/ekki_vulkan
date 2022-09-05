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

    [[nodiscard]] int get_num_active_objects() const { return num_active_objects; }
    [[nodiscard]] const std::string& get_active_object_name_by_index(uint32_t i) const;
    [[nodiscard]] std::shared_ptr<Triangle> get_active_object_by_index(uint32_t i) const { return active_objects[i]; }
    [[nodiscard]] int get_active_object_id_by_index(uint32_t i) const { return num_active_objects; }
private:
    std::list<std::unique_ptr<Animation>> animation_queue{std::list<std::unique_ptr<Animation>>()};
    int num_active_objects = 0;
    std::vector<std::shared_ptr<Triangle>> active_objects{};
};

}
