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

struct VBSout
{
	float4 color : COLOR;
	float4 position : SV_Position;
};

VBSout main(float3 position : POSITION, float4 color : COLOR)
{
	VBSout vbs;
	vbs.position = mul(float4(position, 1.0f), modelView);
	vbs.color = color;
	return vbs;
}