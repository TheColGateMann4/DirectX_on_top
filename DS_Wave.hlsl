#include "VS_TransformConstBuffer.hlsli"

struct DS_OUTPUT
{
	float4 position  : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
	float4 position : SV_POSITION; 
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

cbuffer timeData : register(b1)
{
	float time;
}

cbuffer waveData : register(b2)
{
	float stepness;
	float speed;
	float scale;
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.position = float4(
		patch[0].position *domain.x+patch[1].position*domain.y+patch[2].position*domain.z);

	Output.position.y = (sin(Output.position.x * stepness + time * speed) - 0.5f) * scale + Output.position.y;

	Output.position = mul(Output.position, modelViewProjection);

	return Output;
}
