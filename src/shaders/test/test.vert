#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 Color;

uniform mat4 projection;
uniform mat4 view;

void main() {
    Color = aPos / 2.0f;
    gl_Position = projection * view * vec4(aPos, 1.0f);
};
