#include "ShadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "TransformConstBuffer.hlsli"

Texture2D t_textureMap : register(t0);
SamplerState s_textureSampler : register(s0);

float4 main(float4 position : SV_POSITION, float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{    
    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, s_depthSampler, depthMapCoords, PCF_level, bias, hardwarePCF);       

    return float4(t_textureMap.Sample(s_textureSampler, textureCoords).rgb * MirrorNumber(shadowLevel, 0.5f), 1.0f);
}