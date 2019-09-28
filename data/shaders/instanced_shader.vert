#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 tex;

layout(location = 3) in vec3 translate_pos;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(translate_pos + position, 1.0);
    frag_color = color;
    frag_tex = tex;
}
