cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelViewProj;
};

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos * 1.05f, 1.0f), modelViewProj);
}