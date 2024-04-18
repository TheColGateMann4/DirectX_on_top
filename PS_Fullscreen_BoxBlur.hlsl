cbuffer ScreenBuffer : register(b0)
{
	int strength;
}

Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);



float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);

	const float divider = (2 * strength + 1) * (2 * strength + 1);

	float2 textureSize;
	t_texture.GetDimensions(textureSize.x, textureSize.y);

	const float heightDifference = 1.0f / textureSize.x;
	const float widthDifference = 1.0f / textureSize.y;
	
	[unroll]
	for(int x = -4; x <= 4; x++)
	{
		if(abs(x) > strength)
			continue;

		[unroll]
		for(int y = -4; y <= 4; y++)
		{
			if(abs(y) > strength)
				continue;

			const float2 offset = float2(widthDifference * x, heightDifference * y);
			const float2 texturePosition = texturePos + offset;
			const float4 textureSample = t_texture.Sample(s_sampler, texturePosition);
			result.rgb = max(result.rgb, textureSample.rgb);
			result.a += textureSample.a;
		}
	}
	
	return float4(result.rgb, result.a / divider);
}