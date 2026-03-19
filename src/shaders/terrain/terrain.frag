#version 330 core

out vec4 FragColor;

in float Height;
in vec3 FragPos;
in vec3 Normal;
in vec3 ViewPos;

vec3 blinnPhong(vec3 albedo, vec3 lightPos, float shininess);

void main() {

    vec3 Color = vec3(1.0f, 0.0f, 1.0f);
    float shininess = 0.0f;

    if (Height < 0.41f) {
        Color = vec3(0.0f, 0.0f, 0.5f);
        shininess = 0.5f;
    } else if (Height < 0.65f) { 
        Color = vec3(0.0f, 0.6f, 0.0f);
        shininess = 0.2f;
    } else if (Height < 0.8f) { 
        Color = vec3(0.3f, 0.2f, 0.0f);
        shininess = 0.2f;
    } else if (Height < 0.9f) {
        Color = vec3(0.3f, 0.3f, 0.3f);
        shininess = 0.2f;
    } else {
        Color = vec3(0.9f);
        shininess = 0.2f;
    }

    vec3 lightPos = vec3(0.0f, 10.0f, 0.0f);

    Color = blinnPhong(Color, lightPos, shininess);
    FragColor = vec4(Color, 1.0f);
}

vec3 blinnPhong(vec3 albedo, vec3 lightPos, float shininess) {

    vec3 ambient = albedo * 0.2f;

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir  = normalize(ViewPos  - FragPos);
    float NdotL   = max(dot(Normal, lightDir), 0.0f);
    vec3 diffuse  = albedo * NdotL;

    vec3 H = normalize(lightDir + viewDir);
    float specAngle = max(dot(H, viewDir), 0.0f);
    vec3 specular = albedo * pow(specAngle, shininess * 4.0f);

    vec3 color = ambient + diffuse + specular;

    return color;
}
