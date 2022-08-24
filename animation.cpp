//
// Created by Varun Malladi on 8/22/22.
//

#include "animation.hpp"
#include "constants.hpp"
#include <iostream>

namespace eklib {

std::unique_ptr<Animation> Animation::create(double duration) {
    return std::make_unique<Animation>(duration);
}

void Animation::add_child(std::unique_ptr<Animation> animation) {
    children.push_back(std::move(animation));
}

/**
 * Calls `update()` on the first child, if one exists.
 */
bool Animation::update() {
    if (children.empty()) { return false; }

    std::unique_ptr<Animation> &current_animation = children.front();

    current_animation->update();

    current_animation->remaining_duration -= MS_PER_UPDATE;
    if (current_animation->remaining_duration < 0) {
        children.pop_front();
        std::cout << "removing animation!\n";
    }

    return true;
}

}

#include <chrono>

int main1() {
    auto scene = eklib::Animation(0.0);

    auto animation1 = eklib::Animation::create(2.0);
    auto animation2 = eklib::Animation::create(4.0);
    auto animation3 = eklib::Animation::create(2.0);

    scene.add_child(std::move(animation1));
    scene.add_child(std::move(animation2));
    scene.add_child(std::move(animation3));

    // main loop
    auto base_time = std::chrono::steady_clock::now();
    double base_diff = 0.0;

    auto prev_time = std::chrono::steady_clock::now();
    double time_diff = 0.0;
    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        time_diff += std::chrono::duration<double>(current_time - prev_time).count();
        base_diff += std::chrono::duration<double>(current_time - prev_time).count();
        prev_time = current_time;

        bool shouldBreak = false;
        while (time_diff >= eklib::MS_PER_UPDATE) {
            time_diff -= eklib::MS_PER_UPDATE;
            if (!scene.update()) {
                shouldBreak = true;
                break;
            }
        }

        if (shouldBreak) { break; }
    }

    std::cout << "done; elapsed time: " << base_diff << " s\n";
}
