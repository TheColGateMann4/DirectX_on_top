#include "ShaderFunctions.hlsli"
#include "FloatHLSLMacros.hlsli"

float GetShadowLevel(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, float4 depthMapCoords : DEPTHTEXCOORD)
{             
    return t_depthMap.SampleCmpLevelZero(s_depthComparisonSampler, depthMapCoords.xyz, length(depthMapCoords.xyz) / 100.0f);
}

float4 CalculateDepthTextureCoords(float3 position, matrix model, matrix shadowPosition)
{
    float4 modelPosition = mul(float4(position, 1.0f), model);
    return mul(modelPosition, shadowPosition);
}