#version 410 core

in vec2 TexCoord;
in vec3 VertexColor;
in vec3 FragNormal;
in vec3 FragPos;
in vec4 FragPosClip;

out vec4 OutColor;
layout(location = 1) out vec3 OutFragNormal;
layout(location = 2) out vec3 OutFragPosition;
layout(location = 3) out float OutDepth;
layout(location = 4) out float OutMetallic;
layout(location = 5) out vec3 OutSpecular;
layout(location = 6) out vec3 OutEmission;

uniform sampler2D Texture;
uniform sampler2D EmissionTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D SpecularTexture;
uniform bool UseVertexColor;
uniform bool UseTexture;
uniform bool UseEmission;
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

    if (UseEmission) {
        OutEmission = texture(EmissionTexture, TexCoord).rgb;
    } else {
        OutEmission = vec3(0.0);
    }

    OutDepth = gl_FragCoord.z;
}
