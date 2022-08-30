//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include "vkbe_device.hpp"
#include "types.hpp"

namespace vkbe {

class VkbeModel {
public:
    VkbeModel(VkbeDevice &device, const std::vector<eklib::Vertex> &vertices);
    ~VkbeModel();

    VkbeModel(const VkbeModel &) = delete;
    VkbeModel &operator=(const VkbeModel &) = delete;

    void bind_to_command_buffer(VkCommandBuffer commandBuffer);
    void draw_to_command_buffer(VkCommandBuffer commandBuffer);
private:
    VkbeDevice &vkbe_device;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    uint32_t vertex_count;

    void create_vertex_buffers(const std::vector<eklib::Vertex> &vertices);
};

}
