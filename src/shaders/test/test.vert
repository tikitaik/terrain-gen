#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out float Height;

uniform mat4 projection;
uniform mat4 view;
uniform sampler2D heightMap;

void main() {

    float height = texture(heightMap, aTexCoords).r;
    if (height > 0.8f) {
        height = pow(height, 3) + 0.3f;
    }
    Height = height;

    vec3 pos = vec3(aPos.x, height * 4.0f, aPos.z);

    gl_Position = projection * view * vec4(pos, 1.0f);
}
