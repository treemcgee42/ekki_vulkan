//
// Created by Varun Malladi on 8/22/22.
//

#include "src/include/animation.hpp"

namespace eklib {

void ScaleIn::update() {
    auto obj = *acting_on;
    obj->scale_absolute(static_cast<float>(std::min(1.0, 1.0 - remaining_duration/total_duration)));
}

std::unique_ptr<ScaleIn> ScaleIn::create(const std::shared_ptr<Triangle>& t, double duration) {
    auto animation = std::make_unique<ScaleIn>(t, duration);
    t->scale_absolute(0.0);

    return animation;
}

// ----- Linear shift

LinearShift::LinearShift(std::shared_ptr<Triangle> t, const glm::vec2& shift, double duration):
    Animation{std::move(t), duration},
    target_position{(*acting_on)->getTranslation() + shift}
{
    translation_func = [=](float t) {
        return (1-t) * (*acting_on)->getTranslation() + t * target_position;
    };
}

void LinearShift::update() {
    (*acting_on)->translate_absolute(
        translation_func(static_cast<float>(
            std::min(1.0, 1.0 - remaining_duration/total_duration)
        ))
    );
}

std::unique_ptr<LinearShift> LinearShift::create(const std::shared_ptr<Triangle>& t, const glm::vec2& target, double duration) {
    auto animation = std::make_unique<LinearShift>(t, target, duration);

    return animation;
}

}
