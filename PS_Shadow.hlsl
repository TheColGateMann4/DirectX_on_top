#include "ShaderFunctions.hlsli"
#include "ShadowFunctions.hlsli"

cbuffer lightBuffer : register(b0)
{
    float3 b_lightPosition;
    
    float3 b_ambient;
    float3 b_lightColor;
    
    float b_diffuseIntensity;
    float b_attenuationConst;
    float b_attenuationLinear;
    float b_attenuationQuadratic;
};

cbuffer objectBuffer : register(b1)
{
    float b_specularIntensity;
    float b_specularPower;
    bool b_normalMapEnabled;
    float b_normalMapWeight;
};

cbuffer transformBuffer : register(b2)
{
    matrix b_model;
    matrix b_modelView;
    matrix b_modelViewProjection;
};

cbuffer shadowCameraBuffer : register(b3)
{
    float cameraNear;
    float cameraFar;
};

Texture2D t_textureMap : register(t0);
Texture2D t_depthMap : register(t2);
SamplerState s_textureSampler : register(s0);
SamplerState s_depthSampler : register(s1);

float4 main(float4 position : SV_POSITION, float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{
    depthMapCoords.xyz = depthMapCoords.xyz / depthMapCoords.w;
    
    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthSampler, depthMapCoords, 4);

    const float3 textureSample = t_textureMap.Sample(s_textureSampler, textureCoords).rgb * shadowLevel;
    
    if(shadowLevel != 0.0f)
    {
        return float4(t_textureMap.Sample(s_textureSampler, textureCoords).rgb * shadowLevel, 1.0f);
    }

    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}