cbuffer constBuffer : register(b0)
{
    matrix modelView;
    matrix modelViewProjection;
};

struct VSOUT
{
    float3 positionRelativeToCamera : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

VSOUT main(float3 position : POSITION, float3 normal : NORMAL)
{
    VSOUT vsout;
    vsout.positionRelativeToCamera = (float3) mul(float4(position, 1.0f), modelView);
    vsout.normal = mul(normal, (float3x3) modelView);
    vsout.position = mul(float4(position, 1.0f), modelViewProjection);
    
    return vsout;
}