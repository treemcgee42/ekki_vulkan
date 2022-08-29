//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include <vector>
#include "vkbe_device.hpp"

namespace vkbe {

struct PipelineConfigInfo {
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;

    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
};

class VkbePipeline {
public:
    VkbePipeline(VkbeDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
    ~VkbePipeline();

    VkbePipeline(const VkbePipeline&) = delete;
    VkbePipeline& operator=(const VkbePipeline&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
private:
    VkbeDevice& vkbe_device;
    VkPipeline graphics_pipeline;
    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;

    static std::vector<char> read_file(const std::string& filepath);
    void create_graphics_pipeline(const std::string& vertFilepath, const std::string& fragFilepath,
                                const PipelineConfigInfo& configInfo);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shaderModule);
};

}
