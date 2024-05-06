#include "VS_ShadowFunctions.hlsli"
#include "VS_TransformConstBuffer.hlsli"

cbuffer shadowBuffer : register(b1)
{
    matrix shadowViewProjection;
};

cbuffer heightMapSettings : register(b2)
{
    float b_mapMultipler;
};

struct VSOUT
{
    float3 viewPosition : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float2 textureCoords : TEXCOORD;
    float4 depthMapCoords : DEPTHTEXCOORD;
    float4 position : SV_POSITION;
};

Texture2D t_heightMap : register(t0);
SamplerState s_sampler : register(s0);

VSOUT main(float3 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 textureCoords : TEXCOORD)
{
    const float heightMapSample = t_heightMap.SampleLevel(s_sampler, textureCoords, 0).r * b_mapMultipler;
    position += normal * heightMapSample;

    VSOUT vsout;
    vsout.viewPosition = (float3) mul(float4(position, 1.0f), modelView);
    vsout.viewNormal = mul(normal, (float3x3) modelView);
    vsout.viewTangent = mul(tangent, (float3x3) modelView);
    vsout.viewBitangent = mul(bitangent, (float3x3)modelView);
    vsout.textureCoords = textureCoords;
    vsout.position = mul(float4(position, 1.0f), modelViewProjection);
    vsout.depthMapCoords = CalculateDepthTextureCoords(position, model, shadowViewProjection);

    return vsout;
}