cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelView;
    matrix modelViewProjection;
};

struct VSOUT
{
    float3 positionRelativeToCamera : POSITION;
    float3 normal : NORMAL;
    float2 textureCoords : TEXCOORD;
    float4 position : SV_POSITION;
};

VSOUT main(float3 position : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD)
{
    VSOUT vsout;
    vsout.positionRelativeToCamera = (float3) mul(float4(position, 1.0f), modelView);
    vsout.normal = normal;
    vsout.textureCoords = textureCoords;
    vsout.position = mul(float4(position, 1.0f), modelViewProjection);
    
    return vsout;
}