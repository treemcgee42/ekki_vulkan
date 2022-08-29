//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "vkbe_render_pass.hpp"

namespace vkbe {

class VkbeRenderer {
public:
    VkbeRenderer(VkbeWindow& window, VkbeDevice& device);
    ~VkbeRenderer();

    VkbeRenderer(const VkbeRenderer &) = delete;
    VkbeRenderer &operator=(const VkbeRenderer &) = delete;

    VkCommandBuffer begin_frame();
    void begin_swap_chain_render_pass(VkCommandBuffer commandBuffer);
    void end_swap_chain_render_pass(VkCommandBuffer commandBuffer);
    void end_frame();
private:
    VkbeWindow& vkbe_window;
    VkbeDevice& vkbe_device;
    std::unique_ptr<VkbeSwapChain> vkbe_swap_chain;
    std::unique_ptr<VkbeRenderPass> vkbe_render_pass;
    uint32_t current_swap_chain_image_index;
    // The maximum number of frames that can be operated on at once. Note that this is
    // not directly related to the number of images in the swap chain.
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t current_frame;
    std::vector<VkCommandBuffer> command_buffers;
    // signal that an image has been acquired from the swap chain
    std::vector<VkSemaphore> image_available_semaphores;
    // signal that rendering has finished and image is ready to be presented
    std::vector<VkSemaphore> render_finished_semaphores;
    // waits until rendering is done so that we can safely re-record to the command buffer
    std::vector<VkFence> command_buffer_rendered_fences;

    void create_command_buffers();
    void create_sync_objects();
    VkResult acquire_next_image_from_swap_chain(uint32_t *imageIndex);
    VkCommandBuffer get_current_command_buffer();
    VkResult submit_command_buffers(const VkCommandBuffer *buffers, const uint32_t *imageIndex);
    void recreate_swap_chain_and_dependencies();


};

}
