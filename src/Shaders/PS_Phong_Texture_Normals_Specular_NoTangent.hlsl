#include "PS_shadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "PS_TransformConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    float b_specularIntensity;
    float b_specularPower;
    bool b_normalMapEnabled;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_uvmapTexture : register(t1);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{

    float3 diffuse, specular;
    
    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, depthMapCoords, c0, c1, pcf);
    
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);

    if(shadowLevel != 0.0f)
    { 
    
        if (b_normalMapEnabled)
        {
            const float3 normalMap = t_uvmapTexture.Sample(s_sampler, textureCoords).rgb;
        
            normal = (normalMap * 2.0f) - 1.0f;
            normal.z = -normal.z;
            normal = mul(normal, (float3x3)b_modelView);
        }
        //normal = normalize(normal);
    
        const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
        const float lengthOfVectorLength = length(VectorLength);
        const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
        const float attenuation = 1.0f / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
	
        diffuse = b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
        const float3 w = normal * dot(VectorLength, normal);
        const float3 r = w * 2.0f - VectorLength;
    
         specular = attenuation * (b_lightColor * b_diffuseIntensity) * b_specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), b_specularPower);
        
        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        specular = diffuse = float3(0.0f, 0.0f, 0.0f);
    }

    return float4(saturate((diffuse + b_ambient) * diffuseSample.rgb + specular), diffuseSample.a);
}