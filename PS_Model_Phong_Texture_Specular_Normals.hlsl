#include "ShaderFunctions.hlsli"
#include "PointLightConstBuffer.hlsli"

cbuffer objectBuffer : register(b1)
{
    float b_specularIntensity;
    float b_specularPower;
    bool b_normalMapEnabled;
    bool b_isRetardedBrickWallWithMessedUpNormals;
};

cbuffer transformBuffer : register(b2)
{
    matrix b_modelView;
    matrix b_modelViewProjection;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_uvmapTexture : register(t2);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float3 viewTangent : TANGENT, float3 viewBitangent : BITANGENT, float2 textureCoords : TEXCOORD) : SV_TARGET
{
    if (b_normalMapEnabled)
    {
        normal = GetNormalInViewSpace(normal, viewTangent, viewBitangent, textureCoords, s_sampler, t_uvmapTexture, b_isRetardedBrickWallWithMessedUpNormals);
    }
    
    const float3 VectorLength = b_viewLightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = GetAttenuation(lengthOfVectorLength, b_attenuationConst, b_attenuationLinear, b_attenuationQuadratic);
    
    const float3 diffuse = GetDiffuse(normal, attenuation, DirectionToLightSource, b_lightColor, b_diffuseIntensity);
    
    const float3 specular = GetSpecular(positionRelativeToCamera, normal, VectorLength, attenuation, b_specularPower, b_lightColor, b_specularIntensity);
    
    return float4(saturate((diffuse + b_ambient) * t_diffuseTexture.Sample(s_sampler, textureCoords).rgb + specular), 1.0f);
}