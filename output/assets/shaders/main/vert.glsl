#version 410 core

layout(location = 0) in vec3 APos;       // Position
layout(location = 1) in vec2 ATexCoord;  // Texture Coordinates
layout(location = 2) in vec3 ANormal;    // Normal
layout(location = 3) in vec3 AColor;     // Vertex Color

out vec2 TexCoord;
out vec3 VertexColor;
out vec3 FragNormal;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main() {
    TexCoord = ATexCoord;  
    VertexColor = AColor; 
    FragNormal = mat3(transpose(inverse(Model))) * ANormal;
    FragPos = vec3(Model * vec4(APos, 1.0));

    gl_Position = Projection * View * Model * vec4(APos, 1.0);
}
