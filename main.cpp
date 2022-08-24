#include "vulkan-backend/first_app.hpp"
#include "engine.hpp"
#include "vulkan-backend/simple_render_system.hpp"
#include "scene.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

void do_scene(eklib::Engine& engine, eklib::Scene& scene) {
    while (!engine.lveWindow.shouldClose()) {
        glfwPollEvents();

        if (auto commandBuffer = engine.lveRenderer.beginFrame()) {
            engine.lveRenderer.beginSwapChainRenderPass(commandBuffer);
            scene.draw(engine, commandBuffer);
            ///simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
            engine.lveRenderer.endSwapChainRenderPass(commandBuffer);
            engine.lveRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(engine.lveDevice.device());
}

int main() {
    eklib::Engine engine{};

    eklib::Scene scene1{};
    eklib::Triangle t1{};
    t1.scale_absolute(0.25);
    t1.translate_absolute(-0.5, 0.5);

    eklib::Triangle t2{};
    t2.scale_absolute(0.5);
    t2.translate_absolute(0.5, -0.5);

    eklib::Triangle t3 = t2;
    t3.scale_absolute(0.25);
    t3.set_color(0.0, 1.0, 0.0, 0.1);


    scene1.active_objects.push_front(t1);
    scene1.active_objects.push_front(t2);
    scene1.active_objects.push_front(t3);

    do_scene(engine, scene1);
}

int main2() {
    lve::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}



