#version 450

layout (set=0, binding=0) uniform UniformObject{
    mat4 model;
    mat4 view;
    mat4 proj;
}ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec2 fragUv;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;

void main() {
    gl_Position = ubo.proj* ubo.view * ubo.model * vec4(inPosition, 1.0);
    position = (ubo.model * vec4(inPosition, 1.0)).xyz;
    normal = normalize(( ubo.model * vec4(inNormal, 1.0f)).xyz);
    fragUv = inUv;
}
