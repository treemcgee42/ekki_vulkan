//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include "vulkan-backend/lve_window.hpp"
#include "vulkan-backend/lve_device.hpp"
#include "vulkan-backend/lve_renderer.hpp"
#include "vulkan-backend/simple_render_system.hpp"
#include "meshes.hpp"

namespace eklib {

///
/// This holds the core components of the program, such as the graphical backend and
/// mesh library.
///
class Engine {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    ~Engine()= default;

    Engine(Engine const&) = delete;
    Engine& operator=(Engine const&) = delete;

    lve::LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    lve::LveDevice lveDevice{lveWindow};
    lve::LveRenderer lveRenderer{lveWindow, lveDevice};
    lve::SimpleRenderSystem render_system{lveDevice, lveRenderer.getSwapChainRenderPass()};
    Meshes mesh_library{lveDevice};

};

}
