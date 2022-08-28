#include "vulkan-backend/first_app.hpp"
#include "engine.hpp"
#include "vulkan-backend/simple_render_system.hpp"
#include "scene.hpp"
#include "constants.hpp"
#include "gui.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <chrono>

void do_scene(eklib::Engine& engine, Gui& gui, eklib::Scene& scene) {
    auto base_time = std::chrono::steady_clock::now();

    auto prev_time = std::chrono::steady_clock::now();
    double time_diff = 0.0;

    while (!engine.lveWindow.shouldClose()) {
        glfwPollEvents();

        auto current_time = std::chrono::steady_clock::now();
        time_diff += std::chrono::duration<double>(current_time - prev_time).count();
        prev_time = current_time;

        // UPDATE ANIMATIONS
        while (time_diff >= eklib::MS_PER_UPDATE) {
            time_diff -= eklib::MS_PER_UPDATE;
            scene.update();
        }

        // GUI
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        // RENDER
        if (auto commandBuffer = engine.lveRenderer.beginFrame()) {
            engine.lveRenderer.beginSwapChainRenderPass(commandBuffer);
            scene.draw(engine, commandBuffer);
            ImGui::Render();
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), engine.lveRenderer.getCurrentCommandBuffer());
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            engine.lveRenderer.endSwapChainRenderPass(commandBuffer);
            engine.lveRenderer.endFrame();
        }
    }

    vkDeviceWaitIdle(engine.lveDevice.device());

    vkDestroyDescriptorPool(engine.lveDevice.device(), gui.imgui_descriptor_pool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

int main() {
    eklib::Engine engine{};
    Gui gui{};
    gui.init_imgui(engine);

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

    do_scene(engine, gui, scene1);
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




