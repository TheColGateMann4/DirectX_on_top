Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	return t_texture.Sample(s_sampler, texturePos);
}