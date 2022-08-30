//
// Created by Varun Malladi on 8/22/22.
//

#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include "vulkan-backend/legacy/lve_model.hpp"

namespace eklib {

class Triangle {
public:
    Triangle() = default;
    ~Triangle() = default;

    static std::shared_ptr<Triangle> create() { return std::make_shared<Triangle>(); }

    float getScale();
    glm::vec2 getTranslation();
    float getTranslationComponent(int i);

    void scale_absolute(float s);
    void scale_relative(float s);
    void translate_absolute(float x, float y);
    void translate_absolute(glm::vec2 v);
    void translate_relative(float x, float y);
    void translate_relative(glm::vec2 v);

    void set_color(float r, float g, float b, float a);
    glm::vec4 get_color();



private:
    float scale{1.0};
    glm::vec2 translation{0.0, 0.0};
    glm::vec4 color{1.0, 0.0, 0.0, 1.0};
};

}
