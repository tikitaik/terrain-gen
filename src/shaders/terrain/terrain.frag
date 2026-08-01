#version 330 core

out vec4 FragColor;

in VS_OUT {
    float height;
    vec2 texCoords;
    vec3 fragPos;
    vec3 normal;
    vec3 viewPos;
} fs_in;

uniform sampler2D waterSampler;
uniform sampler2D grassSampler;
uniform sampler2D dirtSampler;
uniform sampler2D stoneSampler;

vec3 blinnPhong(vec3 albedo, vec3 lightPos, float shininess);

void main() {

    vec3 color = vec3(1.0f, 0.0f, 1.0f);
    float shininess = 0.0f;
    
    if (fs_in.height < 0.41f) {
        color = texture(waterSampler, fs_in.texCoords).rgb;
        shininess = 0.5f;
    } else if (fs_in.height < 0.65f) { 
        color = texture(grassSampler, fs_in.texCoords).rgb;
        shininess = 0.03f;
    } else if (fs_in.height < 0.8f) { 
        color = texture(dirtSampler, fs_in.texCoords).rgb;
        shininess = 0.01f;
    } else if (fs_in.height < 0.9f) {
        color = texture(stoneSampler, fs_in.texCoords).rgb;
        shininess = 0.1f;
    } else {
        color = vec3(0.9f);
        shininess = 0.3f;
    }

    //color = vec3(0.2f, 0.08f, 0.03f) * 1.5f;

    vec3 lightPos = vec3(0.0f, 50.0f, 0.0f);

    FragColor = vec4(blinnPhong(color, lightPos, shininess), 1.0f);
    //FragColor = vec4(vec3(fs_in.height), 1.0f);
}

vec3 blinnPhong(vec3 albedo, vec3 lightPos, float shininess) {

    vec3 normal = normalize(fs_in.normal);

    vec3 ambient = albedo * 0.1f;

    vec3 lightDir = normalize(lightPos - fs_in.fragPos);
    vec3 viewDir  = normalize(fs_in.viewPos  - fs_in.fragPos);
    float NdotL   = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse  = albedo * NdotL;

    vec3 H = normalize(lightDir + viewDir);
    float specAngle = max(dot(H, viewDir), 0.0f);
    vec3 specular = albedo * pow(specAngle, shininess * 16.0f);

    vec3 color = ambient + diffuse + specular;

    return color;
}
