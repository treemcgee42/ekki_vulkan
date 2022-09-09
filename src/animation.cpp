//
// Created by Varun Malladi on 9/5/22.
//

#include "src/include/animation.hpp"

namespace eklib {

void Animation::populate(AnimationCreateInfo &info) {
    start_time = info.start_time;
    end_time = info.end_time;
    object_id = info.initial_state->get_id();
    initial_state = info.initial_state;
    final_state = info.final_state;
}

void LinearShift::change_initial_state(const std::shared_ptr<Triangle>& new_initial_state) {
    initial_state = new_initial_state;

    auto final_state_ = Triangle::copy(initial_state);
    final_state_->translate_absolute(target_position);
    final_state = final_state_;
}

LinearShift::LinearShift(std::shared_ptr<Triangle> &initial_state, glm::vec2 target_position_, float start_time_, float end_time_):
    target_position{target_position_}
{
    auto final_state = Triangle::copy(initial_state);
    final_state->translate_absolute(target_position);

    AnimationCreateInfo create_info;
    create_info.start_time = start_time_;
    create_info.end_time = end_time_;
    create_info.initial_state = initial_state;
    create_info.final_state = final_state;

    populate(create_info);
}

Triangle LinearShift::get_object(float at_time) {
    assert(at_time >= start_time && at_time <= end_time);

    // Calculate position
    auto normalized_time = (at_time - start_time) / (end_time - start_time);
    auto new_position = (1-normalized_time) * initial_state->getTranslation() + (normalized_time) * final_state->getTranslation();

    auto to_draw = *initial_state;
    to_draw.translate_absolute(new_position);

    return to_draw;
}

class ConcreteLinearShift: public LinearShift {
public:
    ConcreteLinearShift(std::shared_ptr<Triangle>& initial_state, glm::vec2 target_position, float start_time_, float end_time_): LinearShift{initial_state, target_position, start_time_, end_time_} {}
};

std::shared_ptr<LinearShift> LinearShift::create(std::shared_ptr<Triangle>& initial_state, glm::vec2 target_position, float start_time_, float end_time_) {
    return std::make_shared<ConcreteLinearShift>(initial_state, target_position, start_time_, end_time_);
}

ScaleTo::ScaleTo(std::shared_ptr<Triangle> &initial_state, float target_scale_, float start_time_, float end_time_):
    target_scale{target_scale_}
{
    auto final_state = Triangle::copy(initial_state);
    final_state->scale_absolute(target_scale);

    AnimationCreateInfo create_info;
    create_info.start_time = start_time_;
    create_info.end_time = end_time_;
    create_info.initial_state = initial_state;
    create_info.final_state = final_state;

    populate(create_info);
}

Triangle ScaleTo::get_object(float at_time) {
    assert(at_time >= start_time && at_time <= end_time);

    // Calculate scale
    auto normalized_time = (at_time - start_time) / (end_time - start_time);
    auto new_scale = (1-normalized_time) * initial_state->getScale() + (normalized_time) * final_state->getScale();

    auto to_draw = *initial_state;
    to_draw.scale_absolute(new_scale);

    return to_draw;
}

class ConcreteScaleTo: public ScaleTo {
public:
    ConcreteScaleTo(std::shared_ptr<Triangle> &initial_state, float target_scale, float start_time_, float end_time_): ScaleTo{initial_state, target_scale, start_time_, end_time_} {}
};

std::shared_ptr<ScaleTo> ScaleTo::create(std::shared_ptr<Triangle> &initial_state, float target_scale,
                                                float start_time_, float end_time_) {
    return std::make_shared<ConcreteScaleTo>(initial_state, target_scale, start_time_, end_time_);
}

void ScaleTo::change_initial_state(const std::shared_ptr<Triangle>& new_initial_state) {
    initial_state = new_initial_state;

    auto final_state_ = Triangle::copy(initial_state);
    final_state_->scale_absolute(target_scale);
    final_state = final_state_;
}

}
