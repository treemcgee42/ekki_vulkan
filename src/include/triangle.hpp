//
// Created by Varun Malladi on 8/22/22.
//

#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <string>
#include <memory>

namespace eklib {

class Triangle {
public:
    Triangle() { object_count++; object_id=object_count; };
    ~Triangle() = default;

    static std::shared_ptr<Triangle> create() { return std::make_shared<Triangle>(); }
    static std::shared_ptr<Triangle> copy(const std::shared_ptr<Triangle>& other);

    [[nodiscard]] float getScale() const { return scale; }
    [[nodiscard]] glm::vec2 getTranslation() const { return translation; }
    [[nodiscard]] float getTranslationComponent(int i) const { return translation[i]; }
    [[nodiscard]] glm::vec4 get_color() const { return color; }
    [[nodiscard]] const std::string& get_name() const { return name; }
    [[nodiscard]] uint32_t get_id() const { return object_id; }

    void scale_absolute(float s);
    void scale_relative(float s);
    void translate_absolute(float x, float y);
    void translate_absolute(glm::vec2 v);
    void translate_relative(float x, float y);
    void translate_relative(glm::vec2 v);

    void set_name(const std::string& name_) { name = name_; }
    void set_color(float r, float g, float b, float a);

private:
    static uint32_t object_count;
    uint32_t object_id;
    std::string name = "untitled";
    float scale{1.0};
    glm::vec2 translation{0.0, 0.0};
    glm::vec4 color{1.0, 0.0, 0.0, 1.0};
};

}
