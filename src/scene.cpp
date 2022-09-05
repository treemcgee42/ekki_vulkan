//
// Created by Varun Malladi on 8/23/22.
//

#include "src/include/scene.hpp"
#include "src/include/constants.hpp"

namespace eklib {

struct SimplePushConstantData {
    glm::mat4 model_matrix{1.f};
    glm::vec4 color{1, 0, 0, 1};
};

void Scene::add_animation(std::unique_ptr<Animation> animation) {
    animation_queue.push_back(std::move(animation));
}

void Scene::add_active_object(const std::shared_ptr<Triangle>& object) {
    active_objects.push_back(object);
    num_active_objects++;
}

const std::string& Scene::get_active_object_name_by_index(uint32_t i) const {
    auto obj = active_objects[i];
    // TODO: handle out of bounds

    return obj->get_name();
}

void Scene::draw(Engine& engine, VkCommandBuffer commandBuffer) {
    //engine.vkbe_render_system.vkbe_pipeline->bind(commandBuffer);

    for (const auto& obj : active_objects) {
        SimplePushConstantData push{};
        glm::mat4 scale_matrix{obj->getScale()};
        scale_matrix[3] = {0, 0, 0, 1};
        glm::mat4 translation_matrix = {
                {1.0, 0, 0, 0},
                {0, 1.0, 0, 0},
                {0, 0, 1.0, 0},
                {obj->getTranslationComponent(0), obj->getTranslationComponent(1), 0, 1}
        };
        push.model_matrix = translation_matrix * scale_matrix;
        push.color = obj->get_color();

        vkCmdPushConstants(commandBuffer, engine.vkbe_render_system.pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(SimplePushConstantData), &push);

        auto model= engine.mesh_library.get_mesh(0);
        model->bind_to_command_buffer(commandBuffer);
        model->draw_to_command_buffer(commandBuffer);
    }
}

/**
 * Represents a "tick" update. According to the game loop model, this tick is always
 * of length `MS_PER_UPDATE`.
 */
void Scene::update() {
    if (animation_queue.empty()) { return; }

    std::unique_ptr<Animation> &current_animation = animation_queue.front();

    current_animation->update();
    current_animation->decrement_remaining_duration(MS_PER_UPDATE);

    if (current_animation->get_remaining_duration() < 0) {
        animation_queue.pop_front();
    }
}

}
