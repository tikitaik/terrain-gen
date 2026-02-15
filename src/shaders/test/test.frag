#version 330 core

out vec4 FragColor;

in float Height;

void main() {

    vec3 Color = vec3(1.0f, 0.0f, 1.0f);

    if (Height < 0.3f) {
        Color = vec3(0.0f, 0.0f, 0.5f);
    } else if (Height < 0.4f) { 
        Color = vec3(0.0f, 0.6f, 0.0f);
    } else if (Height < 0.5f) { 
        Color = vec3(0.3f, 0.2f, 0.0f);
    } else if (Height < 0.65f) {
        Color = vec3(0.3f, 0.3f, 0.3f);
    } else {
        Color = vec3(0.9f);
    }

    FragColor = vec4(Color, 1.0f);
}
