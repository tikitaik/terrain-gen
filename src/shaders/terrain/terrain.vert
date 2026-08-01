#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    float height;
    vec2 texCoords;
    vec3 fragPos;
    vec3 normal;
    vec3 viewPos;
} vs_out;


uniform sampler2D heightMap;
uniform float SCALE;
uniform float HEIGHT_TEX_RES;
uniform float PICT_TEX_TILES_PER_SIDE;
uniform float TRI_SIZE;
uniform vec3 viewPos;
uniform mat4 projection;
uniform mat4 view;

void main() {

    // mult by scale so that it scales with the map size too
    float AMPLITUDE = 200.0f * SCALE;
    float DIST = TRI_SIZE / HEIGHT_TEX_RES;

    float height = texture(heightMap, aTexCoords).r;
    float heightNorth = texture(heightMap, aTexCoords + vec2(0, DIST)).r;
    float heightWest  = texture(heightMap, aTexCoords - vec2(DIST, 0)).r;

    if (height < 0.4f) height = 0.4f;
    if (heightNorth < 0.4f) heightNorth = 0.4f;
    if (heightWest < 0.4f) heightWest = 0.4f;

    vs_out.normal = normalize(cross(vec3(0, heightNorth - height, DIST),
                vec3(DIST, heightWest - height, 0)));

    vs_out.height = height;
    vs_out.texCoords = aTexCoords * PICT_TEX_TILES_PER_SIDE;
    vs_out.fragPos = vec3(aPos.x, AMPLITUDE * (height - 0.5f), aPos.z);
    vs_out.viewPos = viewPos;

    gl_Position = projection * view * vec4(vs_out.fragPos, 1.0f);
}
