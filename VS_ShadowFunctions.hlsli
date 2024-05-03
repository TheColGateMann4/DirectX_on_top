float4 CalculateDepthTextureCoords(float3 position, matrix model, matrix shadowPosition)
{
    float4 modelPosition = mul(float4(position, 1.0f), model);
    return mul(modelPosition, shadowPosition);
}