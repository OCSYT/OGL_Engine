#version 410 core

in vec2 TexCoord;
in vec3 VertexColor;
in vec3 FragNormal;
in vec3 FragPos;

out vec4 OutColor;
layout(location = 1) out vec3 OutFragNormal;
layout(location = 2) out vec3 OutFragPosition;
layout(location = 3) out float OutMetallic;
layout(location = 4) out vec3 OutSpecular; 

uniform sampler2D Texture;
uniform sampler2D MetallicTexture;
uniform sampler2D SpecularTexture;
uniform bool UseTexture;
uniform bool UseVertexColor;
uniform bool UseMetallic;
uniform bool UseSpecular;
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

    if (OutColor.a < 0.1) {
        discard;
    }

    float metallic = 0.0;
    vec3 specular = vec3(0);


    if (UseMetallic) {
        metallic = texture(MetallicTexture, TexCoord).r;
    }

    if (UseSpecular) {
        specular = texture(SpecularTexture, TexCoord).rgb;
    }

    OutMetallic = metallic;
    OutSpecular = specular;

    OutFragNormal = normalize(FragNormal);
    OutFragPosition = FragPos;
}
