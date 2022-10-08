//
// Created by Varun Malladi on 8/23/22.
//

#include "src/include/scene.hpp"
#include "src/include/constants.hpp"

#include <iostream>

namespace eklib {

struct SimplePushConstantData {
    glm::mat4 model_matrix{1.f};
    glm::vec4 color{1, 0, 0, 1};
};

Scene::Scene(float duration_):
    duration{duration_},
    current_time{0},
    timeline{std::make_unique<Timeline>(duration_)}
{}

void Scene::add_animation(std::shared_ptr<Animation> animation) {
    timeline->insert(animation);
}


const char* Scene::get_active_object_name_by_index(uint32_t i) const {
    assert(i < num_active_objects);

    return active_objects[i].get_name().c_str();
}

void Scene::draw(Engine& engine, VkCommandBuffer commandBuffer) {
    //engine.vkbe_render_system.vkbe_pipeline->bind(commandBuffer);

    for (const auto& obj : active_objects) {
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

        vkCmdPushConstants(commandBuffer, engine.vkbe_render_system.pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);

        auto model= engine.mesh_library.get_mesh(0);
        model->bind_to_command_buffer(commandBuffer);
        model->draw_to_command_buffer(commandBuffer);
    }
}

void Scene::adjust_current_time(float new_time) {
    if (current_time == new_time) { return; }

    active_objects.clear();
    num_active_objects = 0;
    auto active_animations = timeline->get_active_animations(new_time);
    for (const auto& animation : active_animations) {
        active_objects.push_back(animation->get_object(new_time));
        num_active_objects++;
    }

    current_time = new_time;
}

void Scene::progress_current_time(float increment) {
    if (!is_playing) { return; }

    auto new_time = std::min(duration, current_time + increment);
    adjust_current_time(new_time);
}

}
