//
// Created by Varun Malladi on 8/22/22.
//

#pragma once

#include <list>
#include <memory>
#include <optional>
#include <utility>
#include "triangle.hpp"

namespace eklib {

/**
 * How to achieve animation of several objects at the same time?
 */
class Animation {
public:
    /**
     * Rarely, if ever, do you want to directly construct an `Animation` object,
     * since to use it you will likely need to call `add_child` on it, and you
     * will need a unique pointer to do that (see `create()`).
     *
     * Perhaps the one exception is in creating the overall scene object.
     */
    explicit Animation(double duration):
        total_duration{duration},
        remaining_duration{duration}
    {}
    explicit Animation(std::shared_ptr<Triangle> obj, double duration):
        acting_on{obj},
        total_duration{duration},
        remaining_duration{duration}
    {}

    Animation(const Animation &) = delete;
    void operator=(const Animation &) = delete;

    virtual void update() {}

    [[nodiscard]] double get_remaining_duration() const { return remaining_duration; }
    void decrement_remaining_duration(double amount) { remaining_duration -= amount; }
private:
    std::list<std::unique_ptr<Animation>> children{std::list<std::unique_ptr<Animation>>()};
protected:
    const std::optional<std::shared_ptr<Triangle>> acting_on;
    const double total_duration;
    double remaining_duration;
};

/**
 * Represents the animation where an objects grows in scale from 0 to its original scale.
 */
class ScaleIn: public Animation {
public:
    ScaleIn(std::shared_ptr<Triangle> t, double duration): Animation{std::move(t), duration} {}

    void update() override;

    /**
     * The object is passed as a const reference because the function itself need not increase the reference
     * count. The subtlety is that the constructor to `Animation` class will copy by value and increase the
     * reference count, since it must retain and mutate the object.
     */
    static std::unique_ptr<ScaleIn> create(const std::shared_ptr<Triangle>& t, double duration);
};

class LinearShift: public Animation {
public:
    LinearShift(std::shared_ptr<Triangle> t, const glm::vec2& shift, double duration);
    static std::unique_ptr<LinearShift> create(const std::shared_ptr<Triangle>& t, const glm::vec2& target, double duration);

    void update() override;
private:
    const glm::vec2 target_position;
    std::function<glm::vec2(float)> translation_func;
};

}
