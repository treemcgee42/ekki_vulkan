//
// Created by Varun Malladi on 8/22/22.
//

#include "triangle.hpp"
#include "meshes.hpp"

namespace eklib {

float Triangle::getScale() { return scale; }
glm::vec2 Triangle::getTranslation() { return translation; }
float Triangle::getTranslationComponent(int i) { return translation[i]; }

void Triangle::scale_absolute(float s) { scale = s; }
void Triangle::scale_relative(float s) { scale = scale * s; }

void Triangle::translate_absolute(float x, float y) { translation = {x, y}; }
void Triangle::translate_absolute(glm::vec2 v) { translation = v; }
void Triangle::translate_relative(float x, float y) { translation += glm::vec2(x, y); }
void Triangle::translate_relative(glm::vec2 v) { translation += v; }

void Triangle::set_color(float r, float g, float b, float a) {
    color = {r, g, b, a};
}

glm::vec4 Triangle::get_color() { return color; }



}
