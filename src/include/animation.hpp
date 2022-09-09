//
// Created by Varun Malladi on 9/5/22.
//

#pragma once

#include "triangle.hpp"

namespace eklib {

struct AnimationCreateInfo {
    float start_time;
    float end_time;
    std::shared_ptr<Triangle> initial_state;
    std::shared_ptr<Triangle> final_state;
};

class Animation {
public:
    Animation(const Animation &) = delete;
    void operator=(const Animation &) = delete;

    /**
     * We return a new `Triangle` instead of a pointer to an existing one because this function will almost
     * always involve calculating new properties of the initial/final state triangles, and we do not want those
     * to change between calls.
     */
    virtual Triangle get_object(float at_time) { return Triangle{}; }

    /**
     * By changing the initial state, this function also modifies other fields accordingly, for example
     * by also changing the final state.
     */
    virtual void change_initial_state(const std::shared_ptr<Triangle>& new_initial_state) {}

    [[nodiscard]] float get_start_time() const { return start_time; }
    [[nodiscard]] float get_end_time() const { return end_time; }
    [[nodiscard]] std::shared_ptr<Triangle> get_final_state() const { return final_state; }
    [[nodiscard]] uint32_t get_object_id() const { return object_id; }

protected:
    Animation() = default;
    void populate(AnimationCreateInfo& info);

    float start_time = -1;
    float end_time = -1;
    uint32_t object_id = -1;
    std::shared_ptr<Triangle> initial_state = nullptr;
    std::shared_ptr<Triangle> final_state = nullptr;
};

class LinearShift: public Animation {
public:
    LinearShift() = delete;
    static std::shared_ptr<LinearShift> create(std::shared_ptr<Triangle>& initial_state, glm::vec2 target_position, float start_time, float end_time);

    void change_initial_state(const std::shared_ptr<Triangle>& new_initial_state) override;

    Triangle get_object(float at_time) override;

protected:
    LinearShift(std::shared_ptr<Triangle>& initial_state, glm::vec2 target_position, float start_time, float end_time);

private:
    glm::vec2 target_position;
};

class ScaleTo: public Animation {
public:
    ScaleTo() = delete;
    static std::shared_ptr<ScaleTo> create(std::shared_ptr<Triangle>& initial_state, float target_scale, float start_time_, float end_time_);

    Triangle get_object(float at_time) override;
    void change_initial_state(const std::shared_ptr<Triangle>& new_initial_state) override;

protected:
    ScaleTo(std::shared_ptr<Triangle>& initial_state, float target_scale, float start_time_, float end_time_);

private:
    float target_scale;
};

}