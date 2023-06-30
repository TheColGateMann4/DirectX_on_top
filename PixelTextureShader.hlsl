Texture2DArray textures : register(t0);
SamplerState samplerState : register(s0);
cbuffer PSconstBuffer : register(b0)
{
    float4 timee;
};

float4 main(float2 textureCoordinates : TEXCOORD) : SV_Target
{
    float frame = timee.x % 61;
    return textures.Sample(samplerState, float3(textureCoordinates, frame));
}