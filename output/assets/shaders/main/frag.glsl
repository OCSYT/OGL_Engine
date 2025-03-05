#version 410 core

in vec2 TexCoord;
in vec3 VertexColor;

out vec4 OutColor;

uniform sampler2D Texture;
uniform bool UseTexture;
uniform bool UseVertexColor;
uniform vec4 Color;

void main() {
    vec4 FinalColor = Color;

    if (UseTexture) {
        FinalColor *= texture(Texture, TexCoord);
    }

    if (UseVertexColor) {
        FinalColor *= vec4(VertexColor, 1.0);
    }

    OutColor = FinalColor;

    // Discard transparent fragments
    if (OutColor.a < 0.1) {
        discard;
    }
}
