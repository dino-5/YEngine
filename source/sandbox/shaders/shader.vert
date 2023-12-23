#version 450

layout (binding=0) uniform PassUniformBuffer{
    mat4 view;
    mat4 proj;
}uboPass;

layout (binding=1) uniform ObjectUniformBuffer {
    mat4 model;
}uboObject;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUv;

layout(location = 0) out vec2 fragUv;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;

void main() {
    gl_Position = uboPass.proj* uboPass.view * uboObject.model * vec4(inPosition, 1.0);
    position = (uboObject.model * vec4(inPosition, 1.0)).xyz;
    normal = normalize(( uboObject.model * vec4(inNormal, 1.0f)).xyz);
    fragUv = inUv;
}
