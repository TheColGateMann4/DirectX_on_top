Texture2D t_texture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float2 texturePos : TEXCOORD, float4 position : SV_POSITION) : SV_TARGET
{
	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int textureWidth, textureHeight;

	t_texture.GetDimensions(textureWidth, textureHeight);
	const float heightDifference = 1.0f / textureHeight;
	const float widthDifference = 1.0f / textureWidth;

	
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			const float2 offset = float2(-1.0f + 1.0f * i, -1.0f + 1.0f * j);
			float multipler = 0.0625f;

			if(offset.x == 0 && offset.y == 0)
				multipler = 0.250f;
			else if(offset.x == 0 || offset.y == 0)
				multipler = 0.125f;						

			const float2 texturePosition = texturePos + widthDifference * offset;
			const float4 textureSample = t_texture.Sample(s_sampler, texturePosition);

			result += textureSample * multipler;
		}
	}
	
	return result;
}