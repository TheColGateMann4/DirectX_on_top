#include "ShaderFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    bool b_normalMapEnabled;
    bool b_normalMapHasAlpha;
    bool b_specularMap;
    bool b_specularMapHasAlpha;
    float b_specularIntensity;
    float4 b_specularColor;
    float b_specularPower;
};

cbuffer transformBuffer : register(b2)
{
    matrix b_modelView;
    matrix b_modelViewProjection;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_uvmapTexture : register(t2);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float3 viewTangent : TANGENT, float3 viewBitangent : BITANGENT, float2 textureCoords : TEXCOORD) : SV_TARGET
{    
    normal = normalize(normal);
    
    if (b_normalMapEnabled)
    {
        normal = GetNormalInViewSpace(normal, normalize(viewTangent), normalize(viewBitangent), textureCoords, s_sampler, t_uvmapTexture);
    }
    
    const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = GetAttenuation(lengthOfVectorLength, b_attenuationConst, b_attenuationLinear, b_attenuationQuadratic);
    
    const float3 diffuse = GetDiffuse(normal, attenuation, DirectionToLightSource, b_lightColor, b_diffuseIntensity);
    
    const float3 specular = GetSpecular(positionRelativeToCamera, normal, VectorLength, attenuation, b_specularPower, b_lightColor, b_diffuseIntensity);
    
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);
    
    const float3 diffesePlusAmbient = diffuse + b_ambient;
    const float3 timesDiffuseSample = diffesePlusAmbient * diffuseSample.rgb;
    const float3 plusSpecular = timesDiffuseSample + specular;
    const float3 saturated = saturate(plusSpecular);
    
    return float4(saturated, diffuseSample.a);
}