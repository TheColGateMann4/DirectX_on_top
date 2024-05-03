#include "PS_ShadowFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"
#include "ShadowResources.hlsli"
#include "PS_TransformConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    bool b_normalMapEnabled;
    bool b_normalMapHasAlpha;
    bool b_specularMapEnable;
    bool b_specularMapHasAlpha;
    bool b_specularMapAlphaChannelOnly;
    float b_specularMapWeight;
    float4 b_specularColor;
    float b_specularPower;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_specularTexture : register(t1);
Texture2D t_uvmapTexture : register(t2);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float3 viewTangent : TANGENT, float3 viewBitangent : BITANGENT, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD ) : SV_TARGET
{   
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);
    normal = normalize(normal);

#ifdef TEXTURE_WITH_MASK
    clip(diffuseSample.a < 0.1f ? -1.0f : 1.0f);
    
    if (dot(normal, positionRelativeToCamera) >= 0.0f)
    {
        normal = -normal;
    }
#endif
    
    float3 diffuse, specular, specularColor;
    
    const float shadowLevel = GetShadowLevel(t_depthMap, s_depthComparisonSampler, depthMapCoords, c0, c1);

    if(shadowLevel != 0.0f)
    {
        if (b_normalMapEnabled)
        {
           normal = GetNormalInViewSpace(normal, normalize(viewTangent), normalize(viewBitangent), textureCoords, s_sampler, t_uvmapTexture);
        }
    
        const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
        const float lengthOfVectorLength = length(VectorLength);
        const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
        const float attenuation = GetAttenuation(lengthOfVectorLength, b_attenuationConst, b_attenuationLinear, b_attenuationQuadratic);
	
        diffuse = GetDiffuse(normal, attenuation, DirectionToLightSource, b_lightColor, b_diffuseIntensity);
    
        specularColor = b_specularColor.rgb * b_specularMapWeight;
        float specularPower = b_specularPower;
    
       if(b_specularMapEnable)
        {
            const float4 specularSample = t_specularTexture.Sample(s_sampler, textureCoords);

            if(b_specularMapAlphaChannelOnly)
            {
                specularPower = specularSample.r;
            }
            else
            {
                specularColor = specularSample.rgb * b_specularMapWeight;

                if(b_specularMapHasAlpha)
                    specularPower = pow(2.0f, specularSample.a * 13.0f);
            }
        }
    
        specular = GetSpecular(positionRelativeToCamera, normal, VectorLength, attenuation, specularPower, b_lightColor, b_diffuseIntensity);
    
        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        specularColor = specular = diffuse = float3(0.0f, 0.0f, 0.0f);
    }
   
    
    return float4(saturate((diffuse + b_ambient) * diffuseSample.rgb + specular * specularColor), 1.0f);
}