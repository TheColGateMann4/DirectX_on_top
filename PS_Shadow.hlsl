float4 main(float3 positionRelativeToCamera : POSITION) : SV_TARGET
{    
    return length(positionRelativeToCamera) / 100.0f + 0.005f;
}