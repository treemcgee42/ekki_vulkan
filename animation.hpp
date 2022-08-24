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

    /**
     * Constructs an instance and returns it wrapped in a unique pointer. Animations typically exist to
     * be placed on a queue. Furthermore, they mutate the object they point to, and so they really should
     * not exist in more than one place at once.
     */
    static std::unique_ptr<Animation> create(double duration);
    static std::unique_ptr<Animation> create2(std::shared_ptr<Triangle> obj, double duration);

    Animation(const Animation &) = delete;
    void operator=(const Animation &) = delete;

    void add_child(std::unique_ptr<Animation> animation);
    virtual bool update();

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

    bool update() override;

    /**
     * The object is passed as a const reference because the function itself need not increase the reference
     * count. The subtlety is that the constructor to `Animation` class will copy by value and increase the
     * reference count, since it must retain and mutate the object.
     */
    static std::unique_ptr<ScaleIn> create_scale_in(const std::shared_ptr<Triangle>& t, double duration);
};



}
