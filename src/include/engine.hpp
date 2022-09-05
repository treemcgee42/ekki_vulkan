//
// Created by Varun Malladi on 8/23/22.
//

#pragma once

#include "meshes.hpp"

#include "vkbe_window.hpp"
#include "vkbe_device.hpp"
#include "vkbe_renderer.hpp"
#include "vkbe_render_system.hpp"

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
