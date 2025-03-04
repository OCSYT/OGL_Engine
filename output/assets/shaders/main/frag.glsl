#version 410 core

in vec2 TexCoord;
in vec3 VertexColor;

out vec4 OutColor;

uniform sampler2D Texture;
uniform bool UseTexture;

void main() {
    if (UseTexture) {
        vec4 TextureColor = texture(Texture, TexCoord);
        OutColor = TextureColor * vec4(VertexColor, 1.0);
        
        if(OutColor.w < 0.1){
            discard;
        }
    } else {
        OutColor = vec4(VertexColor, 1.0);
    }
}
