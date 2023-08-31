float4 main(uint tid : SV_PrimitiveID) : SV_TARGET
{
    float2x4 color = { float4(0.0f, 0.0f, 1.0f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f) };
    return color[(tid / 2) % 2];
}