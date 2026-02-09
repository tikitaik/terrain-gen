#version 330 core

out float FragColor;

float rand(vec2 u){
    return fract(sin(dot(u.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

uniform vec2 offset;

void main() {
    vec2 pos = (gl_FragCoord / 64.0f).xy;
    FragColor = rand(pos);
}
