cbuffer ScreenBuffer : register(b0)
{
	int strength;
}

Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float divider = (2 * strength + 1) * (2 * strength + 1);

	int textureWidth, textureHeight;
	t_texture.GetDimensions(textureWidth, textureHeight);

	const float heightDifference = 1.0f / textureHeight;
	const float widthDifference = 1.0f / textureWidth;
	
	for(int x = -strength; x <= strength; x++)
	{
		for(int y = -strength; y <= strength; y++)
		{
			const float2 offset = float2(widthDifference * x, widthDifference * y);
			const float2 texturePosition = texturePos + offset;
			const float4 textureSample = t_texture.Sample(s_sampler, texturePosition);
			result.rgb = max(result.rgb, textureSample.rgb);
			result.a += textureSample.a;
		}
	}
	
	return float4(result.rgb, result.a / divider);
}