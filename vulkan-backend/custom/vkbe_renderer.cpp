//
// Created by Varun Malladi on 8/29/22.
//

#include "vkbe_renderer.hpp"

namespace vkbe {

// ----- Helper functions ----------------------------------------------------

void VkbeRenderer::create_command_buffers() {
    command_buffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkbe_device.get_command_pool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

    vkbe_check_vk_result_panic(
        vkAllocateCommandBuffers(vkbe_device.get_logical_device(), &allocInfo, command_buffers.data()),
        "failed to allocate command buffers!"
    );
}

void VkbeRenderer::create_sync_objects() {
    image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    command_buffer_rendered_fences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vkbe_device.get_logical_device(), &semaphoreInfo, nullptr, &image_available_semaphores[i]) !=
            VK_SUCCESS ||
            vkCreateSemaphore(vkbe_device.get_logical_device(), &semaphoreInfo, nullptr, &render_finished_semaphores[i]) !=
            VK_SUCCESS ||
            vkCreateFence(vkbe_device.get_logical_device(), &fenceInfo, nullptr, &command_buffer_rendered_fences[i]) != VK_SUCCESS) {
            throw std::runtime_error("[vkbe::Vulkan] failed to create synchronization objects for a frame!");
        }
    }
}

/**
 * Signals an image available semaphore and puts the image index in the pointer location.
 */
VkResult VkbeRenderer::acquire_next_image_from_swap_chain(uint32_t *imageIndex) {
    return vkAcquireNextImageKHR(
        vkbe_device.get_logical_device(),
        vkbe_swap_chain->get_swap_chain(),
        std::numeric_limits<uint64_t>::max(),
        image_available_semaphores[current_frame],  // must be a not signaled semaphore
        VK_NULL_HANDLE,
        imageIndex
    );
}

/**
 * Waits for command_buffer_rendered fence
 */
VkCommandBuffer VkbeRenderer::get_current_command_buffer() {
    // This fence is signaled during queue submission in `submit_command_buffers()`
    vkWaitForFences(
        vkbe_device.get_logical_device(),
        1,
        &command_buffer_rendered_fences[current_frame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max()
    );

    return command_buffers[current_frame];
}

/**
 * It is assumed that the image index in the parameter is up to date. For example,
 * `acquire_next_image_from_swap_chain()` should be called prior to this.
 */
VkResult VkbeRenderer::submit_command_buffers(const VkCommandBuffer *buffers, const uint32_t *imageIndex) {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Wait until the image is available before executing commands
    VkSemaphore waitSemaphores[] = {image_available_semaphores[current_frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    // Signal this semaphore when the commands have finished executing (done rendering)
    VkSemaphore signalSemaphores[] = {render_finished_semaphores[current_frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Reset fence so that it may be signaled by the subsequent few lines of code
    vkResetFences(vkbe_device.get_logical_device(), 1, &command_buffer_rendered_fences[current_frame]);

    // The following synchronization is occurring:
    // -- command execution waits on the image available semaphore
    // -- command completion signals
    // ---- render finished semaphore
    // ---- command buffer rendered fence
    vkbe_check_vk_result_panic(
        vkQueueSubmit(vkbe_device.get_graphics_queue(), 1, &submitInfo, command_buffer_rendered_fences[current_frame]),
        "failed to submit draw command buffer!"
    );

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Wait until image is done rendering before presenting
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vkbe_swap_chain->get_swap_chain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(vkbe_device.get_present_queue(), &presentInfo);

    return result;
}

void VkbeRenderer::recreate_swap_chain_and_dependencies() {
    auto prev_swap_chain_image_count = vkbe_swap_chain->get_swap_chain_image_count();

    auto prev_swap_chain = std::move(vkbe_swap_chain);
    vkbe_swap_chain = VkbeSwapChain::recreate_swap_chain(vkbe_window, vkbe_device, std::move(prev_swap_chain));

    vkbe_render_pass = std::make_unique<VkbeRenderPass>(vkbe_device, *vkbe_swap_chain);

    if (prev_swap_chain_image_count != vkbe_swap_chain->get_swap_chain_image_count()) {
        // TODO: do we need to set any signals?
        create_sync_objects();
    }
}

// ----- Core functions ------------------------------------------------------

VkbeRenderer::VkbeRenderer(VkbeWindow& window, VkbeDevice& device):
    vkbe_window{window},
    vkbe_device{device}
{
    vkbe_swap_chain = std::make_unique<VkbeSwapChain>(vkbe_device, vkbe_window.get_extent());
    vkbe_render_pass = std::make_unique<VkbeRenderPass>(vkbe_device, *vkbe_swap_chain);
    create_command_buffers();
    create_sync_objects();
}

// ----- Public functions ----------------------------------------------------

/**
 * Gets image and command buffer for next image in swap chain in a synchronized manner.
 * Begins command buffer recording.
 */
VkCommandBuffer VkbeRenderer::begin_frame() {
    // synchronized
    auto result = acquire_next_image_from_swap_chain(&current_swap_chain_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain_and_dependencies();

        // TODO: is this the right behavior?
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // synchronized
    auto commandBuffer = get_current_command_buffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void VkbeRenderer::begin_swap_chain_render_pass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vkbe_render_pass->get_render_pass();
    renderPassInfo.framebuffer = vkbe_render_pass->get_frame_buffer(current_swap_chain_image_index);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkbe_swap_chain->get_swap_chain_extent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vkbe_swap_chain->get_swap_chain_extent().width);
    viewport.height = static_cast<float>(vkbe_swap_chain->get_swap_chain_extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, vkbe_swap_chain->get_swap_chain_extent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VkbeRenderer::end_swap_chain_render_pass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

void VkbeRenderer::end_frame() {
    auto commandBuffer = get_current_command_buffer();

    vkbe_check_vk_result_panic(vkEndCommandBuffer(commandBuffer), "failed to record command buffer!");

    auto result = submit_command_buffers(&commandBuffer, &current_swap_chain_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkbe_window.was_resized()) {
        vkbe_window.reset_resized_flag();
        recreate_swap_chain_and_dependencies();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

}
