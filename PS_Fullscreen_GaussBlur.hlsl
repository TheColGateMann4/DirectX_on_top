cbuffer CooficientsBuffer : register(b0)
{
	int numberUsed;
	float cooficient[15];
}
cbuffer BlurSettings : register(b1)
{
	bool horizontal;
}

Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	float2 textureSize;
	t_texture.GetDimensions(textureSize.x, textureSize.y);
	
	float horizontalDistance, verticalDistance;

	if(horizontal)
	{
		textureSize.x = 1.0f / textureSize.x;
		textureSize.y = 0.0f;
	}
	else
	{
		textureSize.x = 0.0f;
		textureSize.y = 1.0f / textureSize.y;
	}

	const int r = numberUsed;
	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	[unroll]
	for(int i = -7; i <= 7; i++)
	{
		if(abs(i) > r)
			continue;

		const float2 texturePosition = texturePos + textureSize * i;
		const float4 textureSample = t_texture.Sample(s_sampler, texturePosition);
		const int absoluteValue = abs(i);
		const float cooficientValue = cooficient[absoluteValue];

		result += textureSample * cooficientValue;
	}
	
	return result;
}