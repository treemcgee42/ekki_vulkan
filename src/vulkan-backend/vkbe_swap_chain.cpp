//
// Created by Varun Malladi on 8/28/22.
//

#include "vkbe_swap_chain.hpp"
#include "imgui_impl_vulkan.h"

#include <utility>

namespace vkbe {

// -----------------------------------------------------------------------------
// ----- Helper functions ------------------------------------------------------
// -----------------------------------------------------------------------------

VkSurfaceFormatKHR VkbeSwapChain::choose_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats)
{
    for (const auto &available_format: available_formats) {
        if (available_format.format == VKBE_CONFIG_SURFACE_FORMAT &&
            available_format.colorSpace == VKBE_CONFIG_SURFACE_COLORSPACE)
        {
            return available_format;
        }
    }

    return available_formats[0];
}

VkPresentModeKHR VkbeSwapChain::choose_present_mode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VKBE_CONFIG_PREFERRED_PRESENT_MODE) {
            return availablePresentMode;
        }
    }

    // TODO: better logical for picking present mode

    // guaranteed to exist
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkbeSwapChain::choose_extent(const VkSurfaceCapabilitiesKHR &capabilities) {
    // Special value indicating we need to pick resolution best matching window within
    // min max image extent bounds
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    // Otherwise, pick the extent of the same dimensions of the window, so long as that is
    // supported
    VkExtent2D actualExtent = window_extent;
    actualExtent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(
                capabilities.maxImageExtent.width,
                actualExtent.width
        )
    );
    actualExtent.height = std::max(
        capabilities.minImageExtent.height,
        std::min(
            capabilities.maxImageExtent.height,
            actualExtent.height
        )
    );

    return actualExtent;
}

// -----------------------------------------------------------------------------
// ----- Core implementations --------------------------------------------------
// -----------------------------------------------------------------------------

DeviceSwapChainSupportDetails DeviceSwapChainSupportDetails::query(VkPhysicalDevice device, VkSurfaceKHR surface) {
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

/**
 * Creates the swap chain but also stores data about it, such as extent, format, and handles to
 * the actual images.
 */
void VkbeSwapChain::create_swap_chain() {
    auto swap_chain_support = DeviceSwapChainSupportDetails::query(vkbe_device.get_physical_device(), vkbe_device.get_vulkan_surface());

    VkSurfaceFormatKHR surfaceFormat = choose_surface_format(swap_chain_support.surface_formats);
    VkPresentModeKHR presentMode = choose_present_mode(swap_chain_support.present_modes);

    VkExtent2D extent = choose_extent(swap_chain_support.capabilities);

    // It is recommended to request at least one more image than the minimum
    uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
    min_image_count = image_count;
    if (swap_chain_support.capabilities.maxImageCount > 0 &&
        image_count > swap_chain_support.capabilities.maxImageCount) {
        image_count = swap_chain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkbe_device.get_vulkan_surface();
    createInfo.minImageCount = image_count;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyInfo indices = vkbe_device.queue_family_info;
    uint32_t queueFamilyIndices[] = {indices.graphics_queue_family_index, indices.present_queue_family_index};

    // TODO: better handling instead of VK_SHARING_MODE_CONCURRENT?
    if (indices.graphics_queue_family_index != indices.present_queue_family_index) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swap_chain_support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = old_swap_chain == nullptr ? VK_NULL_HANDLE : old_swap_chain->swap_chain;

    vkbe_check_vk_result_panic(
        vkCreateSwapchainKHR(vkbe_device.get_logical_device(), &createInfo, nullptr, &swap_chain),
        "failed to create swap chain!"
    );

    // We only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images, then resize the container and finally call it again to retrieve the handles.
    vkGetSwapchainImagesKHR(vkbe_device.get_logical_device(), swap_chain, &image_count, nullptr);
    swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(vkbe_device.get_logical_device(), swap_chain, &image_count, swap_chain_images.data());

    swap_chain_image_format = surfaceFormat.format;
    swap_chain_extent = extent;
}

void VkbeSwapChain::create_swap_chain_image_views() {
    swap_chain_image_views.resize(swap_chain_images.size());
    for (size_t i = 0; i < swap_chain_images.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swap_chain_images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swap_chain_image_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vkbe_device.get_logical_device(), &viewInfo, nullptr, &swap_chain_image_views[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

void VkbeSwapChain::init() {
    create_swap_chain();
    create_swap_chain_image_views();
}

VkbeSwapChain::VkbeSwapChain(const VkbeDevice& vkbe_device_, VkExtent2D window_extent_):
    vkbe_device{vkbe_device_},
    window_extent{window_extent_}
{
    init();
}

VkbeSwapChain::VkbeSwapChain(const VkbeDevice& vkbe_device_, VkExtent2D window_extent_, std::shared_ptr<VkbeSwapChain> old_swap_chain_):
    vkbe_device{vkbe_device_},
    window_extent{window_extent_},
    old_swap_chain{std::move(old_swap_chain_)}
{
    init();

    old_swap_chain = nullptr;
}

VkbeSwapChain::~VkbeSwapChain() {
    for (auto image_view : swap_chain_image_views) {
        vkDestroyImageView(vkbe_device.get_logical_device(), image_view, nullptr);
    }
    swap_chain_image_views.clear();

    if (swap_chain != nullptr) {
        vkDestroySwapchainKHR(vkbe_device.get_logical_device(), swap_chain, nullptr);
        swap_chain = nullptr;
    }
}

// -----------------------------------------------------------------------------
// ----- Public functions ------------------------------------------------------
// -----------------------------------------------------------------------------

std::unique_ptr<VkbeSwapChain> VkbeSwapChain::recreate_swap_chain(VkbeWindow& vkbe_window, VkbeDevice& vkbe_device, std::unique_ptr<VkbeSwapChain> vkbe_swap_chain) {
    auto extent = vkbe_window.get_extent();
    while (extent.width == 0 || extent.height == 0) {
        extent = vkbe_window.get_extent();
        glfwWaitEvents();
    }

    // TODO: is this the ideal synchronization?
    vkDeviceWaitIdle(vkbe_device.get_logical_device());

    std::shared_ptr<VkbeSwapChain> oldSwapChain = std::move(vkbe_swap_chain);
    auto new_vkbe_swap_chain = std::make_unique<VkbeSwapChain>(vkbe_device, extent, oldSwapChain);

    ImGui_ImplVulkan_SetMinImageCount(new_vkbe_swap_chain->get_min_image_count());

    // TODO: handle changed formats
//    if (!oldSwapChain->compareSwapFormats(*lveSwapChain.get())) {
//        throw std::runtime_error("Swap chain image (or depth) format has changed!");
//    }

    return std::move(new_vkbe_swap_chain);
}

}
