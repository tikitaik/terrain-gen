#version 330 core

out float FragColor;

uniform float TEX_RES;

uniform vec2  posOffset;
uniform float timeOffset;


float domainWarpFBM(vec2 st);
float fbm(vec2 st);
float perlin(vec2 st);
float ridge(vec2 st);
float turbulence(vec2 st);

float fade(float a);
float rand(vec2 st);
vec2 rand2(vec2 p, float timeOffset);

void main() {

    vec2 st = (gl_FragCoord.xy) / TEX_RES;
    st += posOffset;

    FragColor = ridge(st) + 0.5f * fbm(st);
    //FragColor = FragColor * 0.5f + 0.5f;
}

float domainWarpFBM(vec2 st) {

    vec2 domWarp = vec2(1.1f, -3.2f);

    float fbm1 = fbm(st);
    float fbm2 = fbm(st + domWarp);

    float fbm3 = fbm(st * 4.0f * fbm1 + vec2(1.7f, 9.2f));
    float fbm4 = fbm(st * 4.0f * fbm2 + vec2(8.3f, 2.8f));
 
    return fbm(vec2(fbm3, fbm4));
}

float fbm(vec2 st) {

    float value = 0.0f;

    int octaves = 5;
    float frequency = 4.0f;
    float lacunarity = 2.0f;
    float persistence = 0.8f;

    vec2 pos = st * frequency;

    for (int i = 0; i < octaves; i++) {

        value += perlin(pos) * persistence;
        pos *= lacunarity;
        persistence *= persistence;
    }

    return value;
}

float perlin(vec2 st) {

    vec2 uv = fract(st);
    vec2 gridVec = floor(st);

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

    return mix(mix(dotBottomLeft, dotBottomRight, u), mix(dotTopLeft, dotTopRight, u), v);
}

float ridge(vec2 st) {

    float offset = 1.0f;
    float value = turbulence(st);
    value = offset - value;
    value = pow(value, 5);
    return value;
}

float turbulence(vec2 st) {

    float amp = 0.5f;
    float frequency = 2.0f;
    float lacunarity = 2.0f;
    int octaves = 5;

    float value = 0;

    st *= frequency;

    for (int i = 0; i < octaves; i++) {
        
        value += amp * abs(perlin(st));
        st *= lacunarity;
        amp *= 0.5f;
    }

    return value;
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
