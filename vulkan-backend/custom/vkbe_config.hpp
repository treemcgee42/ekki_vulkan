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

#ifdef NDEBUG
    void vkbe_check_vk_result_panic(VkResult result, const char **msg) {}
#else
/**
 * This "panic" version of Vulkan result checking means that the program will terminate
 * upon failure. Presumably, this is not always desirable. The only "successful" result
 * is considered to be `VK_SUCCESS`.
 */
    void vkbe_check_vk_result_panic(VkResult result, const char *msg) {
        if (result == VK_SUCCESS) { return; }

        std::ostringstream error_msg;
        error_msg << "[vkbe::Vulkan] error: VkResult = " << result << ". " << msg << "\n";

        throw std::runtime_error(error_msg.str());
    }
#endif

const std::vector<const char *> VKBE_CONFIG_VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};
// Window extensions, such as those from GLFW, are not included here.
const std::vector<const char *> VKBE_CONFIG_INSTANCE_EXTENSIONS = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        // TODO: maybe only necessary on macOS
        "VK_KHR_portability_subset"
};
const std::vector<const char *> VKBE_CONFIG_DEVICE_EXTENSIONS = {
#ifdef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        // TODO: maybe only necessary on macOS
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
};

// Application info
const std::string VKBE_CONFIG_APP_NAME = "ekki animation app";
const std::array VKBE_CONFIG_APP_VERSION = {1, 0, 0};
const std::string VKBE_CONFIG_APP_ENGINE_NAME = "no engine";
const std::array VKBE_CONFIG_APP_ENGINE_VERSION = {1, 0, 0};
const uint32_t VKBE_CONFIG_API_VERSION = VK_API_VERSION_1_0;

}