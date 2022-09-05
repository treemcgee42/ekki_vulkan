//
// Created by Varun Malladi on 8/29/22.
//

#include "vkbe_model.hpp"

namespace vkbe {

void VkbeModel::create_vertex_buffers(const std::vector<eklib::Vertex> &vertices) {
    vertex_count = static_cast<uint32_t>(vertices.size());
    assert(vertex_count >= 3 && "Vertex count must be at least 3");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertex_count;

    vkbe_device.create_buffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                           vertex_buffer, vertex_buffer_memory);

    void *data;
    vkMapMemory(vkbe_device.get_logical_device(), vertex_buffer_memory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(vkbe_device.get_logical_device(), vertex_buffer_memory);
}

VkbeModel::VkbeModel(VkbeDevice &device, const std::vector<eklib::Vertex> &vertices):
    vkbe_device{device}
{
    create_vertex_buffers(vertices);
}

VkbeModel::~VkbeModel() {
    vkDestroyBuffer(vkbe_device.get_logical_device(), vertex_buffer, nullptr);
    vkFreeMemory(vkbe_device.get_logical_device(), vertex_buffer_memory, nullptr);
}

void VkbeModel::bind_to_command_buffer(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertex_buffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void VkbeModel::draw_to_command_buffer(VkCommandBuffer commandBuffer) {
    vkCmdDraw(commandBuffer, vertex_count, 1, 0, 0);
}

}
