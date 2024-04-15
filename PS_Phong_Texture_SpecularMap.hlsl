#include "ShadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "TransformConstBuffer.hlsli"

Texture2D t_diffuseTexture : register(t0);
Texture2D t_specularTexture : register(t1);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{
    float3 diffuse, specular, specularColor;
    
    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, s_depthSampler, depthMapCoords, PCF_level, bias, hardwarePCF, circleFilter, radius);

    if(shadowLevel != 0.0f)
    {
        const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
        const float lengthOfVectorLength = length(VectorLength);
        const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
        const float attenuation = 1 / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
	
        diffuse = b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
        const float3 w = normal * dot(VectorLength, normal);
        const float3 r = w * 2.0f - VectorLength;
    
        const float4 specularSample = t_specularTexture.Sample(s_sampler, textureCoords);
        specularColor = specularSample.rgb;
        const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    
        specular = attenuation * (b_lightColor * b_diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), specularPower);
        
        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        specularColor = specular = diffuse = float3(0.0f, 0.0f, 0.0f);
    }
    
    return float4(saturate((diffuse + b_ambient) * t_diffuseTexture.Sample(s_sampler, textureCoords).rgb + specular * specularColor), 1.0f);
}