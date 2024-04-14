cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelView;
    matrix modelViewProjection;
};

cbuffer shadowBuffer : register(b1)
{
    matrix shadowViewProjection;
};

cbuffer scaleConstBuffer : register(b2)
{
    float scaleFactor;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos * scaleFactor, 1.0f), modelViewProjection);
}