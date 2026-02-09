#version 330 core

out float FragColor;

uniform float timeOffset;

const float RESOLUTION = 512.0f;
float GRID_SIZE = 8.0f;

float fade(float a);
float perlinNoise(int octaves);
float rand(vec2 st);
vec2 rand2(vec2 p, float timeOffset);

void main() {
    FragColor = perlinNoise(5);
}

float perlinNoise(int octaves) {
    
    float result = 0.0f;

    for (int i = 0; i < octaves; i++) {

        float scale = 0.8f / pow(2, i);
        float CELL_SIZE = floor(RESOLUTION / GRID_SIZE) * scale;

        vec2 pos = (gl_FragCoord.xy) / CELL_SIZE;

        vec2 uv = fract(pos);
        vec2 gridVec = floor(pos);

        vec2 bottomLeft  = gridVec;
        vec2 bottomRight = gridVec + vec2(1.0f, 0.0f);
        vec2 topLeft     = gridVec + vec2(0.0f, 1.0f);
        vec2 topRight    = gridVec + vec2(1.0f, 1.0f);

        vec2 randBottomLeft  = rand2(bottomLeft, timeOffset);
        vec2 randBottomRight = rand2(bottomRight, timeOffset);
        vec2 randTopLeft     = rand2(topLeft, timeOffset);
        vec2 randTopRight    = rand2(topRight, timeOffset);

        float dotBottomLeft  = dot(uv,  randBottomLeft);
        float dotBottomRight = dot(uv - vec2(1.0f, 0.0f), randBottomRight);
        float dotTopLeft     = dot(uv - vec2(0.0f, 1.0f), randTopLeft);
        float dotTopRight    = dot(uv - vec2(1.0f, 1.0f), randTopRight);

        float u = fade(uv.x);
        float v = fade(uv.y);
        result += scale * mix(mix(dotBottomLeft, dotBottomRight, u), mix(dotTopLeft, dotTopRight, u), v);
    }

    return result * 0.5f + 0.5f;
}

float fade(float t) {
    return ((6 * t - 15) * t + 10) * t * t * t;
}

float rand(vec2 st){
    return fract(sin(dot(st.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 rand2(vec2 p, float timeOffset){
    float a = fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
    float b = fract(sin(dot(p, vec2(269.5, 183.3))) * 43758.5453);
    vec2 grad = sin(vec2(a, b));
    grad *= 43758.5453;
    return sin(grad + timeOffset);
}
