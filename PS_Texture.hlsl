Texture2D textures : register(t0);
SamplerState samplerState : register(s0);

float4 main(float2 textureCoordinates : TEXCOORD) : SV_Target
{
    return textures.Sample(samplerState, textureCoordinates);
}