#version 450

layout (binding=0) uniform PassUniformBuffer{
    mat4 view;
    mat4 proj;
}uboPass;

layout (binding=1) uniform ObjectUniformBuffer {
    mat4 model;
}uboObject;

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = uboPass.proj* uboPass.view * uboObject.model * vec4(inPosition, 1.0);
}