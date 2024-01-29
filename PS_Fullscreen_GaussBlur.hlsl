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
	float textureWidth, textureHeight;
	t_texture.GetDimensions(textureWidth, textureHeight);
	
	float horizontalDistance, verticalDistance;

	if(horizontal)
	{
		horizontalDistance = 1.0f / textureWidth;
		verticalDistance = 0.0f;
	}
	else
	{
		horizontalDistance = 0.0f;
		verticalDistance = 1.0f / textureHeight;
	}

	const int r = numberUsed;
	float3 result = float3(0.0f, 0.0f, 0.0f);
	
	for(int i = -r; i <= r; i++)
	{
		const float2 offset = float2(horizontalDistance * i, verticalDistance * i);
		const float2 texturePosition = texturePos + offset;
		const float3 textureSample = t_texture.Sample(s_sampler, texturePosition).rgb;
		const float cooficientValue = cooficient[abs(i)];
		result += textureSample * cooficientValue;
	}
	
	return float4(result.rgb, 1.0f);
}