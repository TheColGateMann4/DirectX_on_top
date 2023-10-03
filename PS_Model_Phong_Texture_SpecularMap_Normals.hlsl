#include "ShaderFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    bool b_normalMapEnabled;
    bool b_specularMapHasAlpha;
    bool b_specularMapEnable;
    float b_specularPower;
    float3 b_specularColor;
    float b_specularMapWeight;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_specularTexture : register(t1);
Texture2D t_uvmapTexture : register(t2);
SamplerState s_sampler : register(s0);

float4 main(
        float3 positionRelativeToCamera : POSITION,
        float3 normal : NORMAL,
        float3 viewTangent : TANGENT,
        float3 viewBitangent : BITANGENT,
        float2 textureCoords : TEXCOORD ) : SV_TARGET
{   
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);
    clip(diffuseSample.a < 0.1f ? -1.0f : 1.0f);
    
    normal = normalize(normal);
    
    if (dot(normal, positionRelativeToCamera) >= 0.0f)
    {
        normal = -normal;
    }
    
    
        if (b_normalMapEnabled)
        {
            normal = GetNormalInViewSpace(normal, normalize(viewTangent), normalize(viewBitangent), textureCoords, s_sampler, t_uvmapTexture);
        }
    
    const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = GetAttenuation(lengthOfVectorLength, b_attenuationConst, b_attenuationLinear, b_attenuationQuadratic);
	
    const float3 diffuse = GetDiffuse(normal, attenuation, DirectionToLightSource, b_lightColor, b_diffuseIntensity);
    
    const float3 specularColor = b_specularColor * b_specularMapWeight;
    float specularPower = b_specularPower;
    
    if (b_specularMapEnable)
    {
        const float4 specularSample = t_specularTexture.Sample(s_sampler, textureCoords);
        const float3 specularColor = specularSample.rgb * b_specularMapWeight;
        
        if (b_specularMapHasAlpha)
            specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    
    const float3 specular = GetSpecular(positionRelativeToCamera, normal, VectorLength, attenuation, specularPower, b_lightColor, b_diffuseIntensity);
   
    
    return float4(saturate((diffuse + b_ambient) * diffuseSample.rgb + specular * specularColor), diffuseSample.a);
}