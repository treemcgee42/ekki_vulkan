//
// Created by Varun Malladi on 8/28/22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <array>

namespace vkbe {

#ifdef NDEBUG
    const bool VKBE_CONFIG_ENABLE_VALIDATION_LAYERS = false;
#else
    const bool VKBE_CONFIG_ENABLE_VALIDATION_LAYERS = true;
#endif

void vkbe_check_vk_result_panic(VkResult result, const char *msg);

const std::vector<const char *> VKBE_CONFIG_VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
// Window extensions, such as those from GLFW, are not included here.
const std::vector<const char *> VKBE_CONFIG_INSTANCE_EXTENSIONS = {
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
};
const std::vector<const char *> VKBE_CONFIG_DEVICE_EXTENSIONS = {
#ifdef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // TODO: maybe only necessary on macOS
        "VK_KHR_portability_subset"
};

// Application info
const std::string VKBE_CONFIG_APP_NAME = "ekki animation app";
const std::array VKBE_CONFIG_APP_VERSION = {1, 0, 0};
const std::string VKBE_CONFIG_APP_ENGINE_NAME = "no engine";
const std::array VKBE_CONFIG_APP_ENGINE_VERSION = {1, 0, 0};
const uint32_t VKBE_CONFIG_API_VERSION = VK_API_VERSION_1_0;

// Swap chain
// Fixed imgui docking issue
//const VkFormat VKBE_CONFIG_SURFACE_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;
const VkFormat VKBE_CONFIG_SURFACE_FORMAT =   VK_FORMAT_B8G8R8A8_UNORM;
const VkColorSpaceKHR VKBE_CONFIG_SURFACE_COLORSPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
const VkPresentModeKHR VKBE_CONFIG_PREFERRED_PRESENT_MODE = VK_PRESENT_MODE_MAILBOX_KHR;

}