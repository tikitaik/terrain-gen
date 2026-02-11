#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D tex;

void main() {
    //vec3 color = texture(tex, TexCoords).rgb;
    vec3 color = vec3(texture(tex, TexCoords).rgb);
    FragColor = vec4(color, 1.0f);
}
