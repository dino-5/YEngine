#version 450

layout(binding = 0) uniform UniformObject{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec2 fragUv;

void main() {
    gl_Position = ubo.proj* ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragUv = inUv;
}
