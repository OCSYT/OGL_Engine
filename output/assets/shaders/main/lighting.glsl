#version 410 core

in vec2 TexCoord;
out vec4 OutColor;

uniform sampler2D AlbedoTexture;
uniform sampler2D NormalTexture;
uniform sampler2D PositionTexture;
uniform sampler2D MetallicTexture;
uniform sampler2D RoughnessTexture;
uniform sampler2D EmissionTexture;

struct PointLight {
    vec3 Position;
    vec3 Color;
    float Intensity;
};

struct DirectionalLight {
    vec3 Direction;
    vec3 Color;
    float Intensity;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float Intensity;
    float Cutoff; // Spotlight cutoff angle
    float OuterCutoff; // Outer spotlight cutoff angle
};

uniform int NumPointLights;
uniform PointLight PointLights[128];
uniform int NumDirectionalLights;
uniform DirectionalLight DirectionalLights[128];
uniform int NumSpotLights;
uniform SpotLight SpotLights[128];
uniform vec3 ViewPosition;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float a) {
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k) {
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
    return ggx1 * ggx2;
}

vec3 ACESFittedTonemap(vec3 color) {
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

vec3 GammaCorrect(vec3 color, float gamma) {
    return pow(color, vec3(1.0 / gamma));
}

void main() {
    // Sample textures
    vec4 AlbedoSample = texture(AlbedoTexture, TexCoord);
    vec3 Albedo = AlbedoSample.rgb;
    float Alpha = AlbedoSample.a;

    if (Alpha < 0.01) {
        discard;
    }

    // Attempt to fetch Normal and Position data
    vec3 Normal = texture(NormalTexture, TexCoord).rgb;
    vec3 Position = texture(PositionTexture, TexCoord).rgb;

    // Check if normal and position data are invalid (e.g., check if they are zero)
    bool hasNormal = length(Normal) > 0.0;
    bool hasPosition = length(Position) > 0.0;

    if (!hasNormal || !hasPosition) {
        // If no normal or position, use full albedo color (simple color output)
        OutColor = vec4(Albedo, Alpha);
        return;
    }

    // Fetch other textures
    float Metallic = clamp(texture(MetallicTexture, TexCoord).r, 0.0, 1.0);
    float Roughness = clamp(1.0 - texture(RoughnessTexture, TexCoord).r, 0.05, 1.0);
    vec3 Emission = texture(EmissionTexture, TexCoord).rgb;

    vec3 V = normalize(ViewPosition - Position);
    vec3 F0 = mix(vec3(0.04), Albedo, Metallic);
    vec3 Lo = vec3(0.0);

    // Point light loop
    for (int i = 0; i < min(NumPointLights, 128); i++) {
        vec3 L = normalize(PointLights[i].Position - Position);
        vec3 H = normalize(V + L);
        float distance = length(PointLights[i].Position - Position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = PointLights[i].Color * PointLights[i].Intensity * attenuation;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(Normal, H, Roughness);
        float G = GeometrySmith(Normal, V, L, Roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(Normal, V), 0.0) * max(dot(Normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic;

        float NdotL = max(dot(Normal, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // Directional light loop
    for (int i = 0; i < min(NumDirectionalLights, 128); i++) {
        vec3 L = normalize(-DirectionalLights[i].Direction);
        vec3 H = normalize(V + L);
        vec3 radiance = DirectionalLights[i].Color * DirectionalLights[i].Intensity;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(Normal, H, Roughness);
        float G = GeometrySmith(Normal, V, L, Roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(Normal, V), 0.0) * max(dot(Normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic;

        float NdotL = max(dot(Normal, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // Spotlight loop
    for (int i = 0; i < min(NumSpotLights, 128); i++) {
        vec3 L = normalize(SpotLights[i].Position - Position);
        vec3 H = normalize(V + L);
        float distance = length(SpotLights[i].Position - Position);
        float attenuation = 1.0 / (distance * distance);

        // Calculate spotlight intensity based on the angle
        float theta = dot(L, normalize(SpotLights[i].Direction));
        float epsilon = SpotLights[i].Cutoff - SpotLights[i].OuterCutoff;
        float intensity = clamp((theta - SpotLights[i].OuterCutoff) / epsilon, 0.0, 1.0);

        vec3 radiance = SpotLights[i].Color * SpotLights[i].Intensity * attenuation * intensity;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(Normal, H, Roughness);
        float G = GeometrySmith(Normal, V, L, Roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(Normal, V), 0.0) * max(dot(Normal, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - Metallic;

        float NdotL = max(dot(Normal, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // Ambient term
    vec3 ambient = vec3(0.03) * Albedo * (1.0 - Metallic);
    vec3 color = ambient + Lo + Emission;

    // Tonemapping and Gamma Correction
    color = ACESFittedTonemap(color);
    color = GammaCorrect(color, 2.2);

    OutColor = vec4(color, Alpha);
}
