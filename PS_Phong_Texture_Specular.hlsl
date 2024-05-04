#include "PS_ShadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "PS_TransformConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    float b_specularIntensity;
    float b_specularPower;
}

Texture2D t_diffuseTexture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);
    clip(diffuseSample.a < 1.0f ? -1.0f : 1.0f);
    
    float3 diffuse, specular;
    
    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, depthMapCoords, c0, c1, pcf);
    
    if(shadowLevel != 0.0f)
    {
        normal = normalize(normal);
        
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