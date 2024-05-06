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
    float3 positionRelativeToCamera : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

Texture2D t_heightMap : register(t0);
SamplerState s_sampler : register(s0);

VSOUT main(float3 position : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD)
{
    const float heightMapSample = t_heightMap.SampleLevel(s_sampler, textureCoords, 0).r * b_mapMultipler;
    position += normal * heightMapSample;

    VSOUT vsout;
    vsout.positionRelativeToCamera = (float3) mul(float4(position, 1.0f), modelView);
    vsout.normal = mul(normal, (float3x3) modelView);
    vsout.position = mul(float4(position, 1.0f), modelViewProjection);

    return vsout;
}