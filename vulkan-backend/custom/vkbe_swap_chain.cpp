//
// Created by Varun Malladi on 8/28/22.
//

#include "vkbe_swap_chain.hpp"

namespace vkbe {

DeviceSwapChainSupportDetails DeviceSwapChainSupportDetails::query(VkPhysicalDevice device, VkSurfaceKHR &surface) {
    DeviceSwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device,
        surface,
        &details.capabilities
    );

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(
        device,
        surface,
        &formatCount,
        nullptr
    );

    if (formatCount != 0) {
        details.surface_formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            surface,
            &formatCount,
            details.surface_formats.data()
        );
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface,
        &presentModeCount,
        nullptr
    );

    if (presentModeCount != 0) {
        details.present_modes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface,
            &presentModeCount,
            details.present_modes.data()
        );
    }

    return details;
}

}
