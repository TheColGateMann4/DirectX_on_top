#include "ShaderFunctions.hlsli"
#include "FloatHLSLMacros.hlsli"

float GetShadowLevelAtOffsetByHardware(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, float3 depthMapCoords : DEPTHTEXCOORD, float depth, float bias)
{
    return t_depthMap.SampleCmpLevelZero(s_depthComparisonSampler, depthMapCoords, depth - bias);
}

float GetShadowLevelAtOffset(TextureCube t_depthMap, SamplerState s_depthSampler, float3 depthMapCoords : DEPTHTEXCOORD, float depth, float bias, int3 offset)
{
    const float4 sample = t_depthMap.Sample(s_depthSampler, depthMapCoords + offset);
    
    return (sample.r <= depth - bias) ? 1.0f : 0.0f;
}
/*

    All this is setup just to not throw errors. The point is to first get TextureCube working, then make our shaders handle that.
    Vertex Shader depthCoords passing math will need to be reworked

*/
float GetShadowLevel(TextureCube t_depthMap, SamplerComparisonState s_depthComparisonSampler, SamplerState s_depthSampler, float4 depthMapCoords : DEPTHTEXCOORD, int samples, float bias, bool byHardware, bool circleFilter, float radius)
{        
    depthMapCoords.xyz = depthMapCoords.xyz / depthMapCoords.w;
    
    if (depthMapCoords.z < 0.0f || depthMapCoords.z > 1.0f)
        return 0.0f;
    
    if (circleFilter)
    {
        float2 textureCoords = depthMapCoords.xy;
        
        {
            float textureWidth, textureHeight;
            t_depthMap.GetDimensions(textureWidth, textureHeight);
        
            float ratio = textureWidth / textureHeight;
        
            if (ratio > 1.0f)
            {
                textureCoords.x = abs(textureCoords.x * ratio - ratio * 0.5f) + 0.5f;
            }
            else if (ratio < 1.0f)
            {
                textureCoords.y = abs(textureCoords.y * ratio - ratio * 0.5f) + 0.5f;
            }
        }
        
        textureCoords.xy = abs(textureCoords.xy - 0.5f);
        
        if (sqrt(pow(textureCoords.x, 2) + pow(textureCoords.y, 2)) > radius / 2)
            return 0.0f;
    }
    
    float result = 0.0f;
    
    float pixelWidth = 0.0f, pixelHeight = 0.0f;
    
    if (!byHardware)
    {
        float width, height;
        t_depthMap.GetDimensions(width, height);
    
        pixelWidth = 0.5f / width;
        pixelHeight = 0.5f / height;
    }
    
    const float depth = depthMapCoords.z;
    
    [unroll]
    for (int x = -4; x <= 4; x++)
    {
        if (abs(x) <= samples)
        {
            [unroll]
            for (int y = -4; y <= 4; y++)
            {
                if (abs(y) <= samples)
                    if (byHardware)
                        result += GetShadowLevelAtOffsetByHardware(t_depthMap, s_depthComparisonSampler, depthMapCoords.xyz, depth, bias);
                    else
                        result += GetShadowLevelAtOffset(t_depthMap, s_depthSampler, depthMapCoords.xyz, depth, bias, float3(depthMapCoords.x + x * pixelWidth, depthMapCoords.y + y * pixelHeight, 0.0f));
            }
        }
    }
    
    result /= ((samples * 2 + 1) * (samples * 2 + 1));
    
    return byHardware ? result : MirrorNumber(result, 0.5f);
}

float4 CalculateDepthTextureCoords(float3 position, matrix model, matrix shadowViewProjection)
{
    float4 modelPosition = mul(float4(position, 1.0f), model);
    float4 positionRelativeToShadowCamera = mul(modelPosition, shadowViewProjection);

    return (positionRelativeToShadowCamera * float4(0.5f, -0.5f, 1.0f, 1.0f)) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * positionRelativeToShadowCamera.w);
}