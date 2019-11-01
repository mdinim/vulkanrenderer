#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 1, binding = 0) uniform Object {
    mat4 model;
} object;
//layout(push_constant) uniform PushConsts {
//    mat4 obj_model;
//} push_consts;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 tex;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * object.model * vec4(position, 1.0);
    frag_color = color;
    frag_tex = tex;
}
