#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 Color;

uniform mat4 projection;
uniform mat4 view;
uniform sampler2D heightMap;

void main() {

    float height = texture(heightMap, aTexCoords).r * 1.0f;
    Color = vec3(0.0f, 0.0f, 0.5f) +  0.4f * vec3(height / 1.0f);
    vec3 pos = aPos;
    pos.y = height;
    gl_Position = projection * view * vec4(pos, 1.0f);
}
