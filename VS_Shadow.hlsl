cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelView;
    matrix modelViewProjection;
};

cbuffer shadowBuffer : register(b1)
{
    matrix shadowViewProjection;
};

struct VSOUT
{
    float4 position : SV_POSITION;
    float3 positionRelativeToCamera : POSITION;
    float3 normal : NORMAL;
    float2 textureCoords : TEXCOORD;
    float4 depthMapCoords : DEPTHTEXCOORD;
};

VSOUT main( float3 position : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD)
{
	VSOUT vsout;

    vsout.position = mul(float4(position, 1.0f), modelViewProjection);
    vsout.positionRelativeToCamera = (float3)mul(float4(position, 1.0f), modelView);

    float4 modelPosition = mul(float4(position, 1.0f), model);
    float4 positionRelativeToShadowCamera = mul(modelPosition, shadowViewProjection);

    vsout.normal = mul(normal, (float3x3)modelView);
    vsout.textureCoords = textureCoords;
    vsout.depthMapCoords = (positionRelativeToShadowCamera * float4(0.5f, -0.5f, 1.0f, 1.0f)) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * positionRelativeToShadowCamera.w);

    return vsout;
}