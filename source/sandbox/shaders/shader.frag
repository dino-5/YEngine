#version 450

layout(location = 0) in vec2 fragUv;
layout(location = 1) in vec3 position;
layout(location = 2) in vec4 lightPosition;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec4 outColor;

layout(  binding = 2) uniform sampler2D texSampler;
layout(  binding = 4) uniform sampler2D depthSampler;

//
layout(  binding=3 ) uniform LightingStruct{
    vec3 pos;
}lighting;
//

void main() {
    vec3 lightVector = normalize(lighting.pos - position);
    float cosineFactor= dot(lightVector, normal);
    outColor = vec4(lighting.pos, 1.f);
    vec3 projCoords = lightPosition.xyz/lightPosition.w;
    projCoords.xy = projCoords.xy *0.5 + 0.5;
    float closestDepth = texture(depthSampler, projCoords.xy).r;
    float currentDepth = projCoords.z;
	//return;
    vec4 red= vec4(0.23, 0.09, 0.44, 1.0);
    if(currentDepth-0.001<closestDepth)
        outColor = clamp(cosineFactor, 0.f, 1.f) *texture(texSampler, fragUv);
    else
        outColor = clamp(cosineFactor, 0.f, 1.f) *texture(texSampler, fragUv) * 0.5;
    //outColor = vec4(closestDepth, closestDepth, closestDepth, 1.0f);
}
