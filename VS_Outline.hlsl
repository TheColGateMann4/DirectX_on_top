cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelViewProj;
};

cbuffer scaleConstBuffer : register(b1)
{
    float scaleFactor;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos * scaleFactor, 1.0f), modelViewProj);
}