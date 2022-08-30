#include "engine.hpp"
#include "vulkan-backend/legacy/simple_render_system.hpp"
#include "scene.hpp"
#include "constants.hpp"

// std
#include <chrono>

void do_scene(eklib::Engine& engine, eklib::Scene& scene) {
    auto base_time = std::chrono::steady_clock::now();

    auto prev_time = std::chrono::steady_clock::now();
    double time_diff = 0.0;

    while (!engine.vkbe_window.should_close()) {
        glfwPollEvents();

        auto current_time = std::chrono::steady_clock::now();
        time_diff += std::chrono::duration<double>(current_time - prev_time).count();
        prev_time = current_time;

        // UPDATE ANIMATIONS
        while (time_diff >= eklib::MS_PER_UPDATE) {
            time_diff -= eklib::MS_PER_UPDATE;
            scene.update();
        }

        // RENDER
        if (auto commandBuffer = engine.vkbe_renderer.begin_frame()) {
            engine.vkbe_renderer.begin_swap_chain_render_pass(commandBuffer);

            scene.draw(engine, commandBuffer);

            engine.vkbe_renderer.end_swap_chain_render_pass(commandBuffer);
            engine.vkbe_renderer.end_frame();
        }
    }

    vkDeviceWaitIdle(engine.vkbe_device.get_logical_device());
}

int main() {
    eklib::Engine engine{};

    eklib::Scene scene1{};

    auto t1 = eklib::Triangle::create();
    t1->scale_absolute(0.25);
    t1->translate_absolute(-0.5, 0.5);

    auto t2 = eklib::Triangle::create();
    t2->scale_absolute(0.5);
    t2->translate_absolute(0.5, -0.5);

    auto t3 = eklib::Triangle::create();
    t3->scale_absolute(0.25);
    t3->set_color(0.0, 1.0, 0.0, 0.1);

    scene1.add_animation(eklib::ScaleIn::create(t3, 5));
    scene1.add_animation(eklib::LinearShift::create(t3, {-0.25, 0.25}, 5));

    scene1.add_active_object(t1);
    scene1.add_active_object(t2);
    scene1.add_active_object(t3);

    do_scene(engine, scene1);
}
