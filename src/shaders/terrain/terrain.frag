#version 330 core

out vec4 Fragcolor;

in VS_OUT {
    float height;
    vec3 fragPos;
    vec3 normal;
    vec3 viewPos;
} fs_in;

vec3 blinnPhong(vec3 albedo, vec3 lightPos, float shininess);

void main() {

    vec3 color = vec3(1.0f, 0.0f, 1.0f);
    float shininess = 0.0f;

    if (fs_in.height < 0.41f) {
        color = vec3(0.0f, 0.0f, 0.5f);
        shininess = 0.5f;
    } else if (fs_in.height < 0.65f) { 
        color = vec3(0.0f, 0.6f, 0.0f);
        shininess = 0.03f;
    } else if (fs_in.height < 0.8f) { 
        color = vec3(0.3f, 0.2f, 0.0f);
        shininess = 0.01f;
    } else if (fs_in.height < 0.9f) {
        color = vec3(0.3f, 0.3f, 0.3f);
        shininess = 0.1f;
    } else {
        color = vec3(0.9f);
        shininess = 0.3f;
    }

    vec3 lightPos = vec3(0.0f, 10.0f, 0.0f);

    color = blinnPhong(color, lightPos, shininess);
    Fragcolor = vec4(color, 1.0f);
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
