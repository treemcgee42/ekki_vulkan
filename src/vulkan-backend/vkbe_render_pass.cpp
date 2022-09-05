//
// Created by Varun Malladi on 8/29/22.
//

#include "vkbe_render_pass.hpp"

namespace vkbe {

// -----------------------------------------------------------------------------
// ----- Helper functions ------------------------------------------------------
// -----------------------------------------------------------------------------

VkFormat find_depth_format(const VkbeDevice& vkbe_device) {
    return vkbe_device.find_supported_format(
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

/**
 * Create and allocate depth images and image views
 */
void VkbeRenderPass::create_depth_resources() {
    VkFormat format = find_depth_format(vkbe_device);
    depth_format = format;
    VkExtent2D swap_chain_extent = vkbe_swap_chain.get_swap_chain_extent();

    auto image_count = vkbe_swap_chain.get_swap_chain_image_count();
    depth_images.resize(image_count);
    depth_image_memories.resize(image_count);
    depth_image_views.resize(image_count);

    for (int i = 0; i < depth_images.size(); i++) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = swap_chain_extent.width;
        imageInfo.extent.height = swap_chain_extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = depth_format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.flags = 0;

        vkbe_device.create_image(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_images[i], depth_image_memories[i]);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depth_images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = depth_format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkbe_check_vk_result_panic(
            vkCreateImageView(vkbe_device.get_logical_device(), &viewInfo, nullptr, &depth_image_views[i]),
            "failed to create texture image view!"
        );
    }
}

VulkanAttachment VkbeRenderPass::create_color_attachment() {
    VulkanAttachment attachment{};

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = vkbe_swap_chain.get_swap_chain_image_format();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // To be picked up by imgui render pass
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachment.description = colorAttachment;
    attachment.reference = colorAttachmentRef;

    return attachment;
}

VulkanAttachment VkbeRenderPass::create_imgui_color_attachment() {
    VulkanAttachment attachment{};

    VkAttachmentDescription colorAttachment = {};
    //colorAttachment.format = vkbe_swap_chain.get_swap_chain_image_format();
    colorAttachment.format = vkbe_swap_chain.get_swap_chain_image_format();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // Don't clear framebuffer-- draw ui over whatever is drawn
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    // Comes from the main drawing pass
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    attachment.description = colorAttachment;
    attachment.reference = colorAttachmentRef;

    return attachment;
}

VulkanAttachment VkbeRenderPass::create_depth_attachment() {
    VulkanAttachment attachment{};

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depth_format;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachment.description = depthAttachment;
    attachment.reference = depthAttachmentRef;

    return attachment;
}

// -----------------------------------------------------------------------------
// ----- Core implementations --------------------------------------------------
// -----------------------------------------------------------------------------

void VkbeRenderPass::allocate_remaining_render_pass_resources() {
    create_depth_resources();
}

void VkbeRenderPass::create_render_pass() {
    auto color_attachment = create_color_attachment();
    auto depth_attachment = create_depth_attachment();

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment.reference;
    subpass.pDepthStencilAttachment = &depth_attachment.reference;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {color_attachment.description, depth_attachment.description};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    vkbe_check_vk_result_panic(
        vkCreateRenderPass(vkbe_device.get_logical_device(), &renderPassInfo, nullptr, &render_pass),
        "failed to create render pass!"
    );
}

void VkbeRenderPass::create_imgui_render_pass() {
    auto color_attachment = create_imgui_color_attachment();

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment.reference;

    VkSubpassDependency dependency = {};
    // Dependency outside render pass, since we want to synchronize with main pass
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    // Wait for previous pixels to be drawn
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Tell whoever is listening that we are done drawing pixels
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;  // TODO: or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &color_attachment.description;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;

    vkbe_check_vk_result_panic(
        vkCreateRenderPass(vkbe_device.get_logical_device(), &info, nullptr, &imgui_render_pass),
        "failed to create imgui render pass!"
    );
}

void VkbeRenderPass::create_frame_buffers() {
    auto image_count = vkbe_swap_chain.get_swap_chain_image_count();
    frame_buffers.resize(image_count);
    for (size_t i = 0; i < image_count; i++) {
        std::array<VkImageView, 2> attachments = {vkbe_swap_chain.get_swap_chain_image_view(i), depth_image_views[i]};

        VkExtent2D swapChainExtent = vkbe_swap_chain.get_swap_chain_extent();
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = render_pass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        vkbe_check_vk_result_panic(
            vkCreateFramebuffer(vkbe_device.get_logical_device(), &framebufferInfo, nullptr, &frame_buffers[i]),
            "failed to create framebuffer!"
        );
    }
}

void VkbeRenderPass::create_imgui_frame_buffers() {
    auto image_count = vkbe_swap_chain.get_swap_chain_image_count();
    imgui_frame_buffers.resize(image_count);
    VkExtent2D swapChainExtent = vkbe_swap_chain.get_swap_chain_extent();

    VkImageView attachment[1];
    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = imgui_render_pass;
    info.attachmentCount = 1;
    info.pAttachments = attachment;
    info.width = swapChainExtent.width;
    info.height = swapChainExtent.height;
    info.layers = 1;

    for (uint32_t i = 0; i < image_count; i++) {
        attachment[0] = vkbe_swap_chain.get_swap_chain_image_view(i);

        vkbe_check_vk_result_panic(
            vkCreateFramebuffer(vkbe_device.get_logical_device(), &info, VK_NULL_HANDLE, &imgui_frame_buffers[i]),
            "failed to create imgui framebuffer!"
        );
    }
}

VkbeRenderPass::VkbeRenderPass(VkbeDevice& vkbe_device_, VkbeSwapChain& vkbe_swap_chain_):
    vkbe_device{vkbe_device_},
    vkbe_swap_chain{vkbe_swap_chain_}
{
    allocate_remaining_render_pass_resources();
    create_render_pass();
    create_imgui_render_pass();
    create_frame_buffers();
    create_imgui_frame_buffers();
}

VkbeRenderPass::~VkbeRenderPass() {
    for (int i = 0; i < depth_images.size(); i++) {
        vkDestroyImageView(vkbe_device.get_logical_device(), depth_image_views[i], nullptr);
        vkDestroyImage(vkbe_device.get_logical_device(), depth_images[i], nullptr);
        vkFreeMemory(vkbe_device.get_logical_device(), depth_image_memories[i], nullptr);
    }

    for (auto framebuffer : frame_buffers) {
        vkDestroyFramebuffer(vkbe_device.get_logical_device(), framebuffer, nullptr);
    }

    for (auto framebuffer : imgui_frame_buffers) {
        vkDestroyFramebuffer(vkbe_device.get_logical_device(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(vkbe_device.get_logical_device(), render_pass, nullptr);
    vkDestroyRenderPass(vkbe_device.get_logical_device(), imgui_render_pass, nullptr);
}

}