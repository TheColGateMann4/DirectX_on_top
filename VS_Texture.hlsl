cbuffer constBuffer : register(b0)
{
    matrix model;
    matrix modelView;
    matrix modelViewProjection;
};

struct VSOUT
{
    float2 texturePos : TEXCOORD;
    float4 pos : SV_Position;
};

VSOUT main(float3 position : POSITION, float2 textureCoordinates : TEXCOORD)
{
	VSOUT vsout;
	vsout.pos = mul(float4(position, 1.0f), modelView);
    vsout.texturePos = textureCoordinates;
	return vsout;
}