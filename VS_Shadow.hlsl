cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelView;
    matrix modelViewProjection;
};

struct VSOUT
{
    float3 positionRelativeToCamera : POSITION;
    float4 position : SV_POSITION;
}; 

VSOUT main( float3 position : POSITION)
{
    VSOUT vsout;

    vsout.positionRelativeToCamera = mul(float4(position, 1.0f), modelView).xyz;
    vsout.position = mul(float4(position, 1.0f), modelViewProjection);

	return vsout;
}