#include "ShaderFunctions.hlsli"
#include "FloatHLSLMacros.hlsli"

float CalculateDepth(float4 positionInShadowSpace, float c0, float c1)
{
    const float3 m = abs(positionInShadowSpace).xyz;
    
    const float axisLength = max(m.x, max(m.y, m.z));
    
    return (c1 * axisLength + c0) / axisLength;
}

float GetShadowLevel(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, float4 depthMapCoords : DEPTHTEXCOORD, float c0, float c1)
{             
    return t_depthMap.SampleCmpLevelZero(s_depthComparisonSampler, depthMapCoords.xyz, CalculateDepth(depthMapCoords, c0, c1));
}