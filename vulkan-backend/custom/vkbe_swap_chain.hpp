//
// Created by Varun Malladi on 8/28/22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace vkbe {

struct DeviceSwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR> present_modes;

    static DeviceSwapChainSupportDetails query(VkPhysicalDevice device, VkSurfaceKHR &surface);
};

}
