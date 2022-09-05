#pragma once

#include <memory>
#include <vector>

#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

namespace lve {

class SimpleRenderSystem {
   public:
    SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects);

    LveDevice &lveDevice;
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<LvePipeline> lvePipeline;
private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);
};

}  // namespace lve