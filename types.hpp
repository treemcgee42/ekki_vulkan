//
// Created by Varun Malladi on 8/29/22.
//

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace eklib {

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static std::vector<VkVertexInputBindingDescription> get_vulkan_binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> get_vulkan_attribute_descriptions();
};

}
