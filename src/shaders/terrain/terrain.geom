// just needed to generate normals
# version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    float height;
    vec3 fragPos;
    vec3 viewPos;
} gs_in[];

out float Height;
out vec3 FragPos;
out vec3 Normal;
out vec3 ViewPos;

vec3 GetNormal();

void main() {
    
    Normal = GetNormal();

    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        Height = gs_in[i].height;
        FragPos = gs_in[i].fragPos;
        ViewPos = gs_in[i].viewPos;
        EmitVertex();
    }
    EndPrimitive();
}

vec3 GetNormal() {
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}
