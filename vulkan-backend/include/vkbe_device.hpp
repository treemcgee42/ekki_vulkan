//
// Created by Varun Malladi on 8/28/22.
//

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#include "vkbe_window.hpp"
#include "vkbe_config.hpp"

namespace vkbe {

/**
 * Stores relevant information about queue families on the working physical device.
 */
struct QueueFamilyInfo {
    uint32_t graphics_queue_family_index;
    uint32_t present_queue_family_index;
    bool graphics_index_valid = false;
    bool present_index_valid = false;

    bool all_queues_valid() { return graphics_index_valid && present_index_valid; }
};

/**
 * Instantiation of this class entails the creation of
 * - Vulkan instance
 * - logical device
 * - command pool
 */
class VkbeDevice {
public:
    QueueFamilyInfo queue_family_info;
    VkPhysicalDeviceProperties physical_device_properties;

    VkbeDevice(VkbeWindow& vkbe_window);
    ~VkbeDevice();

    VkbeDevice(const VkbeDevice &) = delete;
    VkbeDevice &operator=(const VkbeDevice &) = delete;

    void create_image(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) const;
    [[nodiscard]] VkFormat find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
    void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

    [[nodiscard]] VkPhysicalDevice get_physical_device() const { return physical_device; }
    [[nodiscard]] VkSurfaceKHR get_vulkan_surface() const { return surface; }
    [[nodiscard]] VkDevice get_logical_device() const { return logical_device; }
    [[nodiscard]] VkCommandPool get_command_pool() const { return command_pool; }
    [[nodiscard]] VkQueue get_graphics_queue() const { return graphics_queue; }
    [[nodiscard]] VkQueue get_present_queue() const { return present_queue; }
    [[nodiscard]] VkInstance get_instance() const { return instance; }
private:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkDevice logical_device;
    // TODO: package into a struct?
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkCommandPool command_pool;

    void create_vulkan_instance();
    void setup_debug_messenger();
    void create_vulkan_surface(GLFWwindow *window);
    void pick_physical_device();
    void create_logical_device();
    void get_queues_from_logical_device();
    void create_command_pool();

    bool is_device_suitable(VkPhysicalDevice device);
    std::vector<const char*> get_required_instance_extensions();
};

}
