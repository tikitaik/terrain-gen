#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    float height;
    vec3 fragPos;
    vec3 normal;
    vec3 viewPos;
} vs_out;


uniform sampler2D heightMap;
uniform float TEX_RES;
uniform float TRI_SIZE;
uniform vec3 viewPos;
uniform mat4 projection;
uniform mat4 view;

void main() {

    float amplitude = 10.0f;

    float height = texture(heightMap, aTexCoords).r;
    float dist = TRI_SIZE / TEX_RES;
    float heightNorth = texture(heightMap, aTexCoords + vec2(0, dist)).r;
    float heightWest  = texture(heightMap, aTexCoords - vec2(dist, 0)).r;
    vs_out.normal = normalize(cross(vec3(0, heightNorth - height, dist), 
                vec3(dist, heightWest - height, 0)));

    vs_out.height = height;
    vs_out.fragPos = vec3(aPos.x, amplitude * (height - 0.5f), aPos.z);
    vs_out.viewPos = viewPos;

    gl_Position = projection * view * vec4(vs_out.fragPos, 1.0f);
}
