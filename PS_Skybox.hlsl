TextureCube t_skyboxTexture : register(t6);
SamplerState s_sampler : register(s0);

float4 main(float3 worldPosition : POSITION) : SV_TARGET
{
	return t_skyboxTexture.Sample(s_sampler, worldPosition);
}