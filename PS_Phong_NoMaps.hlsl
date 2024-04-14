#include "ShadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "TransformConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    float4 b_materialColor;
    float4 b_specularColor;
    float b_specularPower;
};

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{
    normal = normalize(normal);
    
    float3 diffuse;
    float4 specular;

    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, s_depthSampler, depthMapCoords, PCF_level, bias, hardwarePCF);       

    if(shadowLevel != 0.0f)
    {
         const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
         const float lengthOfVectorLength = length(VectorLength);
         const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
         const float attenuation = 1.0f / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
    
         diffuse = b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
        const float3 w = normal * dot(VectorLength, normal);
        const float3 r = w * 2.0f - VectorLength;
    
        specular = attenuation * (float4(b_lightColor, 1.0f) * b_diffuseIntensity) * b_specularColor * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), b_specularPower);


        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        diffuse = float3(0.0f, 0.0f, 0.0f);
        specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    return saturate(float4(diffuse + b_ambient, 1.0f) * b_materialColor + specular);
}