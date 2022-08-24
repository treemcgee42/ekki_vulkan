//
// Created by Varun Malladi on 8/23/22.
//

#include "scene.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace eklib {

struct SimplePushConstantData {
    glm::mat4 model_matrix{1.f};
    glm::vec4 color{1, 0, 0, 1};
};

void Scene::draw(Engine& engine, VkCommandBuffer commandBuffer) {
    engine.render_system.lvePipeline->bind(commandBuffer);

    for (Triangle obj : active_objects) {
        SimplePushConstantData push{};
        glm::mat4 scale_matrix{obj.getScale()};
        scale_matrix[3] = {0, 0, 0, 1};
        glm::mat4 translation_matrix = {
                {1.0, 0, 0, 0},
                {0, 1.0, 0, 0},
                {0, 0, 1.0, 0},
                {obj.getTranslationComponent(0), obj.getTranslationComponent(1), 0, 1}
        };
        push.model_matrix = translation_matrix * scale_matrix;
        push.color = obj.get_color();

        //std::cout << glm::to_string(push.model_matrix) << "\n";

        vkCmdPushConstants(commandBuffer, engine.render_system.pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);

        auto model= engine.mesh_library.get_mesh("triangle");
        model->bind(commandBuffer);
        model->draw(commandBuffer);
    }
}

}
