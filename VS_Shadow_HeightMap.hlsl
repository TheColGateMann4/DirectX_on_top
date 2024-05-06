#include "VS_TransformConstBuffer.hlsli"

cbuffer heightMapSettings : register(b2)
{
    float b_mapMultipler;
};

Texture2D t_heightMap : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float3 position : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD) : SV_POSITION
{
    const float heightMapSample = t_heightMap.SampleLevel(s_sampler, textureCoords, 0).r * b_mapMultipler;
    position += normal * heightMapSample;

    return mul(float4(position, 1.0f), modelViewProjection);
}