#version 450

layout(location = 0) in vec2 fragUv;
layout(location = 1) in vec3 position;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;


void main() {                    
    outColor = texture(texSampler, fragUv);
	//outColor = vec4(1.0f, 1.f, 0.f ,1.f);
	
}
