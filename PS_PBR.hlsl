cbuffer lightBuffer : register(b0)
{
    float3 b_viewLightPosition;
    
    float3 b_ambient;
    float3 b_lightColor;
    
    float b_diffuseIntensity;
    float b_attenuationConst;
    float b_attenuationLinear;
    float b_attenuationQuadratic;
};

cbuffer ObjectBuffer : register(b1)
{
    float b_roughness;
    float b_metalic;
    float3 b_diffuseColor;
    float3 b_emission;
    float3 b_reflectivity;
};

#include "ShaderFunctions.hlsli"

static const float _Pi = 3.14159265358979323846f;

float NormalDist(const float alpha, const float3 N, const float3 H)
{
    const float numerator = pow(alpha, 2.0f);
    const float3 NdotH = max(dot(N, H), 0.0f);

    float denominator = _Pi * pow(pow(NdotH, 2.0f) * (pow(alpha, 2.0f) - 1.0f) + 1.0f, 2.0f);
    denominator = max(denominator, 0.000001f);

    return numerator / denominator;
}

float GeometryShadowingFunction(const float alpha, const float3 N, const float3 X)
{
    const float numerator = max(dot(N, X), 0.0f);
    const float k = alpha / 2.0f;

    float denominator = max(dot(N, X), 0.0f) * (1.0f - k) + k;
    denominator = max(denominator, 0.000001f);

    return numerator / denominator;
}

float GeometryShadow(const float alpha, const float3 N, const float3 V, const float3 L)
{
    return GeometryShadowingFunction(alpha, N, V) * GeometryShadowingFunction(alpha, N, L);
}

float3 FrenelsEquation(const float3 F0, const float3 V, const float3 H)
{
    return F0 + (float3(1.0f, 1.0f, 1.0f) - F0) * pow(1.0f - max(dot(V, H), 0.0f), 5.0f);
}

float3 GetPBR(const float3 N, const float3 V, const float3 L, const float3 H, const float3 reflectivity, const float3 emission, const float3 diffuseColor, const float metalic, const float roughness)
{
    const float alpha = pow(roughness, 2.0f);
    const float3 Ks = FrenelsEquation(reflectivity, V, H);
    const float3 Kd = (float3(1.0f, 1.0f, 1.0f) - Ks) * (1.0f - metalic);
    
    const float3 lambert = diffuseColor / _Pi;
    
    const float3 cookTorranceNumerator = NormalDist(alpha, N, H) * GeometryShadow(alpha, N, V, L) * FrenelsEquation(reflectivity, V, H);
    float cookTorranceDenominator = 4.0f * max(dot(V, N), 0.0f) * max(dot(L, N), 0.0f);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001f);
    const float3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    const float3 BRDF = Kd * lambert + Ks * cookTorrance;

    const float lengthOfDistanceToLight = length(L - V);

    const float atteniuation = GetAttenuation(lengthOfDistanceToLight, b_attenuationConst, b_attenuationLinear, b_attenuationQuadratic);

    const float3 outgoingLight = (((float3(1.0f, 1.0f, 1.0f) + emission) * BRDF) * b_lightColor * max(dot(L, N), 0.0f)) * atteniuation;

    return saturate(outgoingLight);
}

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL) : SV_TARGET
{
	const float3 N = normalize(normal);
    const float3 V = normalize(-positionRelativeToCamera);
    const float3 L = normalize(b_viewLightPosition - positionRelativeToCamera);
    const float3 H = normalize(V + L);

    return float4(GetPBR(N, V, L, H, b_reflectivity, b_emission, b_diffuseColor, b_metalic, b_roughness), 1.0f);
}