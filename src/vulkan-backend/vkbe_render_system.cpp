//
// Created by Varun Malladi on 8/29/22.
//

#include "vkbe_render_system.hpp"
#include <glm/glm.hpp>

namespace vkbe {

// todo: move elsewhere
struct SimplePushConstantData {
    glm::mat4 model_matrix{1.f};
    glm::vec4 color{1, 0, 0, 1};
};

void VkbeRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    vkbe_check_vk_result_panic(
        vkCreatePipelineLayout(vkbe_device.get_logical_device(), &pipelineLayoutInfo, nullptr, &pipeline_layout),
        "failed to create pipeline layout!"
    );
}

void VkbeRenderSystem::create_pipeline(VkRenderPass renderPass) {
    assert(pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    VkbePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipeline_layout;

    vkbe_pipeline = std::make_unique<VkbePipeline>(vkbe_device, "src/shaders/simple_shader.vert.spv","src/shaders/simple_shader.frag.spv", pipelineConfig);
}

VkbeRenderSystem::VkbeRenderSystem(VkbeDevice &device, VkRenderPass renderPass):
    vkbe_device{device}
{
    createPipelineLayout();
    create_pipeline(renderPass);
}

VkbeRenderSystem::~VkbeRenderSystem() {
    vkDestroyPipelineLayout(vkbe_device.get_logical_device(), pipeline_layout, nullptr);
}

}