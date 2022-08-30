//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include "vulkan-backend/legacy/lve_window.hpp"
#include "vulkan-backend/legacy/lve_device.hpp"
#include "vulkan-backend/legacy/lve_renderer.hpp"
#include "vulkan-backend/legacy/simple_render_system.hpp"
#include "meshes.hpp"

#include "vulkan-backend/include/vkbe_window.hpp"
#include "vulkan-backend/include/vkbe_device.hpp"
#include "vulkan-backend/include/vkbe_renderer.hpp"
#include "vulkan-backend/include/vkbe_render_system.hpp"

namespace eklib {

///
/// This holds the core components of the program, such as the graphical backend and
/// mesh library.
///
class Engine {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    ~Engine() = default;

    //Engine(Engine const&) = delete;
    //Engine& operator=(Engine const&) = delete;

    vkbe::VkbeWindow vkbe_window{WIDTH, HEIGHT, "Hello Vulkan!"};
    vkbe::VkbeDevice vkbe_device{vkbe_window};
    vkbe::VkbeRenderer vkbe_renderer{vkbe_window, vkbe_device};
    vkbe::VkbeRenderSystem vkbe_render_system{vkbe_device, vkbe_renderer.get_render_pass()};
    Meshes mesh_library{vkbe_device};
};

}
