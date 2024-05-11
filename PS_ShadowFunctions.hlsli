#include "ShaderFunctions.hlsli"
#include "FloatHLSLMacros.hlsli"

float CalculateDepth(float3 positionInShadowSpace, float c0, float c1)
{
    const float3 m = abs(positionInShadowSpace).xyz;
    
    const float axisLength = max(m.x, max(m.y, m.z));
    
    return (c1 * axisLength + c0) / axisLength;
}

float GetShadowDepthAtOffset(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, float4 depthMapCoords : DEPTHTEXCOORD, float c0, float c1, int2 offset)
{
    uint width, height;

    t_depthMap.GetDimensions(width, height);
    
    //we are checking for longset axis to determine the ratios
    const float3 lengthOfAxels = abs(depthMapCoords.xyz);
    const float longestAxis = max(lengthOfAxels.x, max(lengthOfAxels.y, lengthOfAxels.z));
    
    //getting ratio space values
    const float3 depthCoordsInRatioSpace = depthMapCoords.xyz / longestAxis;
    
    //we are using ratio space just for adding offset which is ratio'd by default
    const float2 offsetInRatioSpace = offset * (1.0f / width);

    //determining where offsets should be added
    float3 personalizedOffsetInRatioSpace = float3(0.0f, 0.0f, 0.0f);
    
    // + or - of values doesn't matter since its + and - will be used anyways.
    // So there is no point in adding more code and time to this problem.
    //
    // If we would do something like this but with care of individual values then this should be taken care of.
    // But we care only about outcome of values, so we are safe
    if (abs(depthMapCoords.x) == longestAxis)
    {
        personalizedOffsetInRatioSpace.y = offsetInRatioSpace.x;
        personalizedOffsetInRatioSpace.z = offsetInRatioSpace.y;
    }
    else if (abs(depthMapCoords.y) == longestAxis)
    {
        personalizedOffsetInRatioSpace.x = offsetInRatioSpace.x;
        personalizedOffsetInRatioSpace.z = offsetInRatioSpace.y;
    }
    else if (abs(depthMapCoords.z) == longestAxis)
    {
        personalizedOffsetInRatioSpace.x = offsetInRatioSpace.x;
        personalizedOffsetInRatioSpace.y = offsetInRatioSpace.y;
    }
   
    const float3 localDepthCoords = (depthCoordsInRatioSpace + personalizedOffsetInRatioSpace) * longestAxis;
    
    return t_depthMap.SampleCmpLevelZero(s_depthComparisonSampler, localDepthCoords, CalculateDepth(localDepthCoords, c0, c1));
}

float GetShadowDepthAtOffsetDebug(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, SamplerState s_sampler, float4 depthMapCoords : DEPTHTEXCOORD, float c0, float c1, int2 offset)
{
    uint width, height;

    t_depthMap.GetDimensions(width, height);
    
    //we are checking for longset axis to determine the ratios
    const float3 lengthOfAxels = abs(depthMapCoords.xyz);
    const float longestAxis = max(lengthOfAxels.x, max(lengthOfAxels.y, lengthOfAxels.z));
    
    //getting ratio space values
    const float3 depthCoordsInRatioSpace = depthMapCoords.xyz / longestAxis;
    
    //we are using ratio space just for adding offset which is ratio'd by default
    const float2 offsetInRatioSpace = offset * (1.0f / width);

    //determining where offsets should be added
    float3 personalizedOffsetInRatioSpace = float3(0.0f, 0.0f, 0.0f);
    
    // + or - of values doesn't matter since its + and - will be used anyways.
    // So there is no point in adding more code and time to this problem.
    //
    // If we would do something like this but with care of individual values then this should be taken care of.
    // But we care only about outcome of values, so we are safe
    if (abs(depthMapCoords.x) == longestAxis)
    {
        personalizedOffsetInRatioSpace.y = offsetInRatioSpace.x;
        personalizedOffsetInRatioSpace.z = offsetInRatioSpace.y;
    }
    else if (abs(depthMapCoords.y) == longestAxis)
    {
        personalizedOffsetInRatioSpace.x = offsetInRatioSpace.x;
        personalizedOffsetInRatioSpace.z = offsetInRatioSpace.y;
    }
    else if (abs(depthMapCoords.z) == longestAxis)
    {
        personalizedOffsetInRatioSpace.x = offsetInRatioSpace.x;
        personalizedOffsetInRatioSpace.y = offsetInRatioSpace.y;
    }
   
    float3 localDepthCoords = (depthCoordsInRatioSpace + personalizedOffsetInRatioSpace) * longestAxis;
    
    float3 aaa = t_depthMap.Sample(s_sampler, localDepthCoords);
    
    if (aaa.x == 1000000.0f)
        localDepthCoords.x += 100000.0f;
    
    return t_depthMap.SampleCmpLevelZero(s_depthComparisonSampler, localDepthCoords, CalculateDepth(localDepthCoords, c0, c1));
}

float GetShadowLevel(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, float4 depthMapCoords : DEPTHTEXCOORD, float c0, float c1, int pcf)
{
    const float divider = (2 * pcf + 1) * (2 * pcf + 1);
    float result = 0.0f;
    
    [unroll]
    for (int x = -5; x <= 5; x++)
    {
        if (abs(x) > pcf)
            continue;
        
        [unroll]
        for (int y = -5; y <= 5; y++)
        {
            if (abs(y) > pcf)
                continue;
        
            result += GetShadowDepthAtOffset(t_depthMap, s_depthComparisonSampler, depthMapCoords, c0, c1, int2(x, y));
        }
    }
    
    return result / divider;
}

float GetShadowDebugLevel(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, SamplerState s_sampler, float4 depthMapCoords : DEPTHTEXCOORD, float c0, float c1, int pcf)
{
    const float divider = (2 * pcf + 1) * (2 * pcf + 1);
    float result = 0.0f;
    
    [unroll]
    for (int x = -5; x <= 5; x++)
    {
        if (abs(x) > pcf)
            continue;
        
        [unroll]
        for (int y = -5; y <= 5; y++)
        {
            if (abs(y) > pcf)
                continue;
        
            result += GetShadowDepthAtOffsetDebug(t_depthMap, s_depthComparisonSampler, s_sampler, depthMapCoords, c0, c1, int2(x, y));
        }
    }
    
    return result / divider;
}