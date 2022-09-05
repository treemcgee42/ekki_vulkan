#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push {
    mat4 model_matrix;
    vec4 color;
} push;

void main() {
    vec4 hom_coord = push.model_matrix * vec4(position, 0.0, 1.0);

    gl_Position = hom_coord;
}