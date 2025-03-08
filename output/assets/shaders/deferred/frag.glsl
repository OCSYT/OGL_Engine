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
layout(location = 5) out float OutRoughness;
layout(location = 6) out vec3 OutEmission;

// Uniforms
uniform sampler2D Texture;
uniform sampler2D EmissionTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D RoughnessTexture;
uniform bool UseVertexColor;
uniform bool UseTexture;
uniform bool UseEmission;
uniform bool UseMetallic;
uniform bool UseRoughness;
uniform vec4 Color;

// Ordered Dither Matrix (4x4 Bayer matrix)
const mat4 ditherMatrix = mat4(
    0.0 / 16.0,  8.0 / 16.0,  2.0 / 16.0, 10.0 / 16.0,
    12.0 / 16.0,  4.0 / 16.0, 14.0 / 16.0,  6.0 / 16.0,
    3.0 / 16.0, 11.0 / 16.0,  1.0 / 16.0,  9.0 / 16.0,
    15.0 / 16.0,  7.0 / 16.0, 13.0 / 16.0,  5.0 / 16.0
);

void main() {
    vec4 FinalColor = Color;

    if (UseTexture) {
        FinalColor *= texture(Texture, TexCoord);
    }

    if (UseVertexColor) {
        FinalColor *= vec4(VertexColor, 1.0);
    }

    vec2 screenPos = FragPosClip.xy / FragPosClip.w;
    ivec2 screenPixel = ivec2(gl_FragCoord.xy);

    // Calculate dither value from the matrix
    float ditherValue = ditherMatrix[screenPixel.x % 4][screenPixel.y % 4];

    if (FinalColor.a < ditherValue) {
        discard;
    }

    OutColor = FinalColor;

    float metallic = 0.0;
    float roughness = 0.0;

    if (UseMetallic) {
        metallic = texture(MetallicTexture, TexCoord).r;
    }

    if (UseRoughness) {
        roughness = texture(RoughnessTexture, TexCoord).r;
    }

    OutMetallic = metallic;
    OutRoughness = roughness;

    OutFragNormal = normalize(FragNormal);
    OutFragPosition = FragPos;

    if (UseEmission) {
        OutEmission = texture(EmissionTexture, TexCoord).rgb;
    } else {
        OutEmission = vec3(0.0);
    }

    OutDepth = gl_FragCoord.z;
}