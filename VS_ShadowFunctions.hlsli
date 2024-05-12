float4 CalculateDepthTextureCoords(float3 position, matrix model, matrix shadowPosition)
{
    float4 modelPosition = mul(float4(position, 1.0f), model);
    return mul(modelPosition, shadowPosition);
}

float3 SampleNearPixels(Texture2D t_texture, SamplerState s_sampler, float2 texturePosition : TEXCOORD, int sampleNumber)
{
    const float divider = (2 * sampleNumber + 1) * (2 * sampleNumber + 1);
    float3 result = float3(0.0f, 0.0f, 0.0f);
    
    float2 textureSize;
    t_texture.GetDimensions(textureSize.x, textureSize.y);

    const float2 pixelSize = 1.0f / textureSize;
    
    [unroll]
    for (int x = -4; x <= 4; x++)
    {
        if (abs(x) > sampleNumber)
            continue;

		[unroll]
        for (int y = -4; y <= 4; y++)
        {
            if (abs(y) > sampleNumber)
                continue;
            
            const float2 offset = float2(pixelSize.x * x, pixelSize.y * y);
            const float2 offsetTexturePosition = texturePosition + offset;
            result += t_texture.SampleLevel(s_sampler, offsetTexturePosition, 0);
        }
    }
    
    return result / divider;
}