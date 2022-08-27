//
// Created by Varun Malladi on 8/23/22.
//

#include "engine.hpp"

namespace eklib {

void Engine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) {
    auto cmd = lveRenderer.unsafe_getCurrentCommandBuffer();
    //begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer for imgui init!");
    };


    function(cmd);


    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer for imgui init!");
    }

    VkSubmitInfo submit{};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    submit.waitSemaphoreCount = 0;
    submit.pWaitSemaphores = nullptr;
    submit.pWaitDstStageMask = nullptr;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;
    submit.signalSemaphoreCount = 0;
    submit.pSignalSemaphores = nullptr;


    //submit command buffer to the queue and execute it.
    // _renderFence will now block until the graphic commands finish execution
    if (vkQueueSubmit(lveDevice.get_graphics_queue(), 1, &submit, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue for imgui init!");
    }
    vkQueueWaitIdle(lveDevice.get_graphics_queue());

    vkResetCommandPool(lveDevice.device(), lveDevice.getCommandPool(), 0);
}

}