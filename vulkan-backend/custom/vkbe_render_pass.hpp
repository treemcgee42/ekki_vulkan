//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "vkbe_device.hpp"
#include "vkbe_swap_chain.hpp"

namespace vkbe {

struct VulkanAttachment {
    VkAttachmentDescription description;
    VkAttachmentReference reference;
};

/**
 * Responsible for the render pass and framebuffers. These two functionalities are coupled
 * because a framebuffer is only used in the context of a render pass.
 */
class VkbeRenderPass {
public:
    VkbeRenderPass(VkbeDevice& vkbe_device_, VkbeSwapChain& vkbe_swap_chain_);
    ~VkbeRenderPass();

    VkbeRenderPass(const VkbeRenderPass &) = delete;
    VkbeRenderPass& operator =(const VkbeRenderPass &) = delete;

    [[nodiscard]] VkRenderPass get_render_pass() const { return render_pass; }
    [[nodiscard]] VkFramebuffer get_frame_buffer(uint32_t i) { return frame_buffers[i]; }
private:
    const VkbeDevice& vkbe_device;
    const VkbeSwapChain& vkbe_swap_chain;
    VkFormat depth_format;
    std::vector<VkImage> depth_images;
    std::vector<VkDeviceMemory> depth_image_memories;
    std::vector<VkImageView> depth_image_views;
    VkRenderPass render_pass;
    std::vector<VkFramebuffer> frame_buffers;

    void allocate_remaining_render_pass_resources();
    void create_render_pass();
    void create_frame_buffers();

    void create_depth_resources();
    VulkanAttachment create_depth_attachment();
    VulkanAttachment create_color_attachment();
};

}
