cbuffer ScreenBuffer : register(b0)
{
	bool horizontal;
	int strength;
}

Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	int textureWidth, textureHeight;
	t_texture.GetDimensions(textureWidth, textureHeight);

	float2 ratioSize;
	
	if(horizontal)
		ratioSize = float2(1.0f / (textureWidth / 2.0f), 0.0f);
	else
		ratioSize = float2(0.0f, 1.0f / (textureHeight / 2.0f));

	[unroll]
	for(int i = -3; i <= 3; i++)
	{
		if(abs(i) > strength)
			continue;

		const float2 offset = ratioSize * i;
		const float2 texturePosition = texturePos + offset;
		const float4 textureSample = t_texture.Sample(s_sampler, texturePosition);
	
		if(textureSample.a > 0.0f)
			return textureSample;
	}
	
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}