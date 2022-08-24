//
// Created by Varun Malladi on 8/22/22.
//

#pragma once

#include <list>
#include <memory>

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
    explicit Animation(double duration): remaining_duration{duration} {}

    static std::unique_ptr<Animation> create(double duration);

    Animation(const Animation &) = delete;
    void operator=(const Animation &) = delete;

    void add_child(std::unique_ptr<Animation> animation);
    virtual bool update();

private:
    double remaining_duration;
    std::list<std::unique_ptr<Animation>> children{std::list<std::unique_ptr<Animation>>()};
};

}
