#include "ShaderFunctions.hlsli"
#include "FloatHLSLMacros.hlsli"

cbuffer lightBuffer : register(b0)
{
    float3 b_lightPosition;
    
    float3 b_ambient;
    float3 b_lightColor;
    
    float b_diffuseIntensity;
    float b_attenuationConst;
    float b_attenuationLinear;
    float b_attenuationQuadratic;
};

cbuffer objectBuffer : register(b1)
{
    float b_specularIntensity;
    float b_specularPower;
    bool b_normalMapEnabled;
    float b_normalMapWeight;
};

cbuffer transformBuffer : register(b2)
{
    matrix b_model;
    matrix b_modelView;
    matrix b_modelViewProjection;
};

Texture2D t_textureMap : register(t0);
Texture2D t_depthMap : register(t2);
SamplerState s_textureSampler : register(s0);
SamplerState s_depthSampler : register(s1);

float4 main(float4 position : SV_POSITION, float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD, float4 depthMapCoords : DEPTHTEXCOORD) : SV_TARGET
{
    float4 result;

    depthMapCoords.xyz = depthMapCoords.xyz / depthMapCoords.w;
    
    if(depthMapCoords.z < 0)
        return float4(0.0f, 0.0f, 0.0f, 1.0f);

    const float4 sample =  t_depthMap.Sample(s_depthSampler, depthMapCoords.xy);
    const float depth = depthMapCoords.z;
    
    bool wrongDepth = sample.r > depthMapCoords.z;
    
    if(!wrongDepth)
    {
        const float valueDifference = abs(sample.r - depthMapCoords.z);
        const float valueNormal = min(abs(sample.r + depthMapCoords.z), FLT_MAX);
        const float epsilon = 128 * FLT_EPSILON;
        
        wrongDepth = valueDifference < max(FLT_MIN, epsilon * valueNormal);
    }

    if(wrongDepth)
    {
        result = float4(b_lightColor, 1.0f);
    }
    else
    {
        result = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }


	return result;
}