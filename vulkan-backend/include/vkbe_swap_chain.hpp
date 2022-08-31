//
// Created by Varun Malladi on 8/28/22.
//

#pragma once

#include "vkbe_window.hpp"
#include "vkbe_device.hpp"

#include <vulkan/vulkan.h>
#include <vector>

namespace vkbe {

struct DeviceSwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR> present_modes;

    static DeviceSwapChainSupportDetails query(VkPhysicalDevice device, VkSurfaceKHR surface);
};

struct SwapChainInfo {
    VkExtent2D extent;
    uint32_t image_count;
    VkFormat image_format;
    std::vector<VkImage> swap_chain_images;
};

class VkbeSwapChain {
public:
    VkbeSwapChain(const VkbeDevice& vkbe_device_, VkExtent2D window_extent);
    VkbeSwapChain(const VkbeDevice& vkbe_device_, VkExtent2D window_extent, std::shared_ptr<VkbeSwapChain> old_swap_chain_);
    ~VkbeSwapChain();

    VkbeSwapChain(const VkbeSwapChain &) = delete;
    VkbeSwapChain &operator=(const VkbeSwapChain &) = delete;

    static std::unique_ptr<VkbeSwapChain> recreate_swap_chain(VkbeWindow& vkbe_window, VkbeDevice& vkbe_device, std::unique_ptr<VkbeSwapChain> vkbe_swap_chain);
    [[nodiscard]] VkExtent2D get_swap_chain_extent() const { return swap_chain_extent; }
    [[nodiscard]] VkFormat get_swap_chain_image_format() const { return swap_chain_image_format; }
    [[nodiscard]] uint32_t get_swap_chain_image_count() const { return swap_chain_images.size(); }
    [[nodiscard]] VkImageView get_swap_chain_image_view(uint32_t i) const { return swap_chain_image_views[i]; }
    [[nodiscard]] VkSwapchainKHR get_swap_chain() const { return swap_chain; }
    [[nodiscard]] uint32_t get_min_image_count() const { return min_image_count; }
    [[nodiscard]] uint32_t get_image_count() const { return swap_chain_images.size(); }
private:
    VkExtent2D window_extent;
    const VkbeDevice& vkbe_device;

    VkSwapchainKHR swap_chain;
    // Shared so that it's dropped when it's no longer in use
    std::shared_ptr<VkbeSwapChain> old_swap_chain;

    uint32_t min_image_count;
    VkExtent2D swap_chain_extent;
    VkFormat swap_chain_image_format;
    std::vector<VkImage> swap_chain_images;
    std::vector<VkImageView> swap_chain_image_views;

    void create_swap_chain();
    void create_swap_chain_image_views();
    void init();

    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats);
    VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D choose_extent(const VkSurfaceCapabilitiesKHR &capabilities);
};

}
