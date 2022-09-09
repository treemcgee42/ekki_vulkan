//
// Created by Varun Malladi on 8/22/22.
//

#include "src/include/triangle.hpp"

namespace eklib {

uint32_t Triangle::object_count = 0;

void Triangle::scale_absolute(float s) { scale = s; }
void Triangle::scale_relative(float s) { scale = scale * s; }

void Triangle::translate_absolute(float x, float y) { translation = {x, y}; }
void Triangle::translate_absolute(glm::vec2 v) { translation = v; }
void Triangle::translate_relative(float x, float y) { translation += glm::vec2(x, y); }
void Triangle::translate_relative(glm::vec2 v) { translation += v; }

void Triangle::set_color(float r, float g, float b, float a) {
    color = {r, g, b, a};
}

std::shared_ptr<Triangle> Triangle::copy(const std::shared_ptr<Triangle>& other) {
    auto the_copy = *other;
    return std::make_shared<Triangle>(the_copy);
}

}
