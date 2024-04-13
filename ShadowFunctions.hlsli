#include "FloatHLSLMacros.hlsli"

float GetShadowLevel(Texture2D t_depthMap, sampler s_depthSampler, float4 depthMapCoords : DEPTHTEXCOORD, int samples)
{
    float result = 0.0f;
    
    float width, height;
    
    t_depthMap.GetDimensions(width, height);
    
    float pixelWidth = 0.5f / width;
    float pixelHeight = 0.5f / height;
    
    for (int x = -samples; x <= samples; x++)
        for (int y = -samples; y <= samples; y++)
        {
            const float4 sample = t_depthMap.Sample(s_depthSampler, float2(depthMapCoords.x + x * pixelWidth, depthMapCoords.y + y * pixelHeight));
            const float depth = depthMapCoords.z;
        
            //if out sample if higher than 1.0f it means that its out of light camera view, then we won't cast a shadow there anyways
            bool occluded = (sample.r > 1.0f) ? false : (sample.r >= depth);
        
            if (!occluded)
            {
                const float valueDifference = abs(sample.r - depth);
                const float valueNormal = min(abs(sample.r + depth), FLT_MAX);
                const float epsilon = 128 * FLT_EPSILON;
            
                occluded = valueDifference < max(FLT_MIN, epsilon * valueNormal);
            }
            
            result += (occluded) ? (1.0f / ((samples * 2 + 1) * (samples * 2 + 1))) : (0.0f);

        }

    return result;
}