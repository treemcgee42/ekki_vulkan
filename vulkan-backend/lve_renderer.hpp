#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_window.hpp"

namespace lve {

class LveRenderer {
   public:
    LveRenderer(LveWindow &window, LveDevice &device);
    ~LveRenderer();

    LveRenderer(const LveRenderer &) = delete;
    LveRenderer &operator=(const LveRenderer &) = delete;

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    VkRenderPass getSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
        return commandBuffers[currentFrameIndex];
    }

    VkCommandBuffer unsafe_getCurrentCommandBuffer() const {
        //assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
        return commandBuffers[currentFrameIndex];
    }

    int getFrameIndex() const {
        assert(isFrameStarted && "Cannot get frame index when frame not in progress");
        return currentFrameIndex;
    }

    [[nodiscard]] size_t get_image_count() const { return lveSwapChain->imageCount(); }

   private:
    LveWindow &lveWindow;
    LveDevice &lveDevice;
    std::unique_ptr<LveSwapChain> lveSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;
    uint32_t currentImageIndex;
    int currentFrameIndex{0};
    bool isFrameStarted{false};

    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();
};

}  // namespace lve