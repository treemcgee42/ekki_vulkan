//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "vkbe_render_pass.hpp"
#include "imgui.h"

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
    void render_imgui_frame(ImDrawData* draw_data);

    VkCommandBuffer get_command_buffer(uint32_t i) { return command_buffers[i]; }
    VkCommandBuffer get_current_command_buffer();
    VkCommandBuffer get_current_imgui_command_buffer();

    [[nodiscard]] VkRenderPass get_render_pass() const { return vkbe_render_pass->get_render_pass(); }
    [[nodiscard]] VkRenderPass get_imgui_render_pass() const { return vkbe_render_pass->get_imgui_render_pass(); }
    [[nodiscard]] uint32_t get_min_swap_chain_image_count() const { return vkbe_swap_chain->get_min_image_count(); }
    [[nodiscard]] uint32_t get_swap_chain_image_count() const { return vkbe_swap_chain->get_image_count(); }
private:
    VkbeWindow& vkbe_window;
    VkbeDevice& vkbe_device;
    std::unique_ptr<VkbeSwapChain> vkbe_swap_chain;
    std::unique_ptr<VkbeRenderPass> vkbe_render_pass;
    uint32_t current_swap_chain_image_index;
    // The maximum number of frames that can be operated on at once. Note that this is
    // not directly related to the number of images in the swap chain.
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t current_frame = 0;
    std::vector<VkCommandBuffer> command_buffers;
    std::vector<VkCommandBuffer> imgui_command_buffers;
    // signal that an image has been acquired from the swap chain
    std::vector<VkSemaphore> image_available_semaphores;
    // signal that non-ui graphics has finished and ui can be drawn
    std::vector<VkSemaphore> non_ui_finished_semaphores;
    // signal that rendering has finished and image is ready to be presented
    std::vector<VkSemaphore> render_finished_semaphores;
    // waits until rendering is done so that we can safely re-record to the command buffer
    std::vector<VkFence> command_buffer_rendered_fences;

    void create_command_buffers();
    void create_imgui_command_buffers();
    void create_sync_objects();
    VkResult acquire_next_image_from_swap_chain(uint32_t *imageIndex);
    VkResult submit_command_buffers(const VkCommandBuffer *buffers, const uint32_t *imageIndex);
    void recreate_swap_chain_and_dependencies();

    VkResult submit_imgui_command_buffers(const VkCommandBuffer *buffers, const uint32_t *imageIndex);


};

}
