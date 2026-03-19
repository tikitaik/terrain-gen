#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    float height;
    vec3 fragPos;
    vec3 viewPos;
} vs_out;

uniform sampler2D heightMap;
uniform vec3 viewPos;
uniform mat4 projection;
uniform mat4 view;

void main() {

    float height = texture(heightMap, aTexCoords).r;
    if (height < 0.4f) {
        height = 0.4f;
    }

    float amplitude = 10.0f;

    vs_out.height = height;
    vs_out.fragPos = vec3(aPos.x, amplitude * height, aPos.z);
    vs_out.viewPos = viewPos;

    gl_Position = projection * view * vec4(vs_out.fragPos, 1.0f);
}
