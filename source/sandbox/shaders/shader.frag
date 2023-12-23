#version 450

layout(location = 0) in vec2 fragUv;
layout(location = 1) in vec3 position;
layout(location = 2) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout(  binding = 2) uniform sampler2D texSampler;

//
layout(  binding=3 ) uniform LightingStruct{
    vec3 pos;
}lighting;
//

void main() {
    vec3 lightVector = normalize(lighting.pos - position);
    float cosineFactor= dot(lightVector, normal);
    outColor = vec4(lighting.pos, 1.f);
	//return;
	outColor = clamp(cosineFactor, 0.f, 1.f) * texture(texSampler, fragUv);
}
