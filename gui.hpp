//
// Created by Varun Malladi on 8/27/22.
//

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "engine.hpp"

class Gui {
public:
    void init_imgui(eklib::Engine& engine);

    VkDescriptorPool imgui_descriptor_pool;
};


