#include "VS_TransformConstBuffer.hlsli"

cbuffer scaleConstBuffer : register(b2)
{
    float scaleFactor;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos * scaleFactor, 1.0f), modelViewProjection);
}