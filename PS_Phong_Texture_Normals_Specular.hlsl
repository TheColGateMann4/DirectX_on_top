#include "ShadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "TransformConstBuffer.hlsli"

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

Texture2D t_diffuseTexture : register(t0);
Texture2D t_uvmapTexture : register(t2);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float3 viewTangent : TANGENT, float3 viewBitangent : BITANGENT, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{    
    normal = normalize(normal);
    
    float3 diffuse, specular;

    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, s_depthSampler, depthMapCoords, PCF_level, bias, hardwarePCF);    
    
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);

    if(shadowLevel != 0.0f)
    {
        if (b_normalMapEnabled)
        {
            normal = GetNormalInViewSpace(normal, normalize(viewTangent), normalize(viewBitangent), textureCoords, s_sampler, t_uvmapTexture);
        }
        
        const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
        const float lengthOfVectorLength = length(VectorLength);
        const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
        
        const float attenuation = GetAttenuation(lengthOfVectorLength, b_attenuationConst, b_attenuationLinear, b_attenuationQuadratic);
    
        diffuse = GetDiffuse(normal, attenuation, DirectionToLightSource, b_lightColor, b_diffuseIntensity);
    
        specular = GetSpecular(positionRelativeToCamera, normal, VectorLength, attenuation, b_specularPower, b_lightColor, b_diffuseIntensity);

        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        specular = diffuse = float3(0.0f, 0.0f, 0.0f);
    }
    
    return float4(saturate((diffuse + b_ambient) * diffuseSample.rgb + specular), diffuseSample.a);
}