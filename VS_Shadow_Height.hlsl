Texture2D t_heightMap : register(t0);
SamplerState s_sampler : register(s0);

struct VSOUT
{
    float3 normal : NORMAL;
    float2 textureCoords : TEXCOORD;
    float4 position : SV_POSITION;
};

VSOUT main(float3 position : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD)
{
    VSOUT vsout;
    vsout.normal = normal;
    vsout.textureCoords = textureCoords;
    vsout.position = float4(position, 1.0f);

    return vsout;
}