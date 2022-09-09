//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include <deque>

#include "timeline.hpp"
#include "triangle.hpp"
#include "engine.hpp"

namespace eklib {

class Scene {
public:
    Scene(float duration_);
    ~Scene() = default;

    /**
     * This adjusts the scene's internal current time variable, and updates any relevant fields. For example,
     * the active animations list is updated to contain the animations which are active at the new current time.
     */
    void adjust_current_time(float new_time);
    /**
     * Increase the current time by some amount. If increasing it would cause the current time to exceed the scene's
     * duration, then make the current time the scene duration (should represent the final frame).
     */
    void progress_current_time(float increment);
    /**
     * Essentially just inserts the animation into the timeline.
     */
    void add_animation(std::shared_ptr<Animation> animation);
    /**
     * Draws all of the active objects, according to the ordering of that underlying data structure.
     */
    void draw(Engine& engine, VkCommandBuffer commandBuffer);

    [[nodiscard]] int get_num_active_objects() const { return num_active_objects; }
    [[nodiscard]] const char* get_active_object_name_by_index(uint32_t i) const;
    // todo: devise a better way of doing this for the deque
    [[nodiscard]] Triangle get_active_object_by_index(uint32_t i) const { return active_objects[i]; }
    [[nodiscard]] int get_active_object_id_by_index(uint32_t i) const { return active_objects[i].get_id(); }
    [[nodiscard]] float get_current_time() const { return current_time; }

private:
    // how long the scene is
    float duration;
    // the scene's representation of how far along it is. This reflects, for example, what active objects are there.
    float current_time;
    std::unique_ptr<Timeline> timeline;
    // these are updated every (new) frame
    int num_active_objects = 0;
    std::deque<Triangle> active_objects{};
};

}
