    #version 410 core

    layout(location = 0) in vec3 APos;
    layout(location = 1) in vec3 AColor;
    layout(location = 2) in vec2 ATexCoord;

    out vec2 TexCoord;
    out vec3 VertexColor;

    uniform mat4 Model;
    uniform mat4 View;
    uniform mat4 Projection;

    void main() {
        TexCoord = vec2(ATexCoord.x, ATexCoord.y);  
        VertexColor = AColor;
        gl_Position = Projection * View * Model * vec4(APos, 1.0);
    }
