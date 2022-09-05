//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include "vkbe_device.hpp"
#include "vkbe_pipeline.hpp"

namespace vkbe {

class VkbeRenderSystem {
public:
    VkbeRenderSystem(VkbeDevice &device, VkRenderPass renderPass);
    ~VkbeRenderSystem();

    VkbeRenderSystem(const VkbeRenderSystem &) = delete;
    VkbeRenderSystem &operator=(const VkbeRenderSystem &) = delete;

    std::unique_ptr<VkbePipeline> vkbe_pipeline;
    VkPipelineLayout pipeline_layout;
private:
    VkbeDevice& vkbe_device;

    void createPipelineLayout();
    void create_pipeline(VkRenderPass renderPass);
};

}
