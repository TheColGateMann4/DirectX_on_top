cbuffer ScreenBuffer : register(b0)
{
	int strength;
}

Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	int textureWidth, textureHeight;
	t_texture.GetDimensions(textureWidth, textureHeight);

	textureWidth = textureWidth / 2;
	textureHeight = textureHeight / 2;

	const float heightDifference = 1.0f / textureHeight;
	const float widthDifference = 1.0f / textureWidth;

	[loop] for(int x = -strength; x <= strength; x++)
	{
		[loop] for(int y = -strength; y <= strength; y++)
		{
			const float2 offset = float2(widthDifference * x, widthDifference * y);
			const float2 texturePosition = texturePos + offset;
			const float4 textureSample = t_texture.Sample(s_sampler, texturePosition);
	
			if(textureSample.a > 0)
				return textureSample;
		}
	}
	
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}