#include "VS_TransformConstBuffer.hlsli"

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 normal : NORMAL;
    float2 textureCoords : TEXCOORD;
    float4 position : SV_POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
};

cbuffer heightMapSettings : register(b1)
{
    float b_mapMultipler;
};

Texture2D t_heightMap : register(t0);
SamplerState s_sampler : register(s0);

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

    const float3 normal = patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;
    const float2 texCoords = patch[0].textureCoords * domain.x + patch[1].textureCoords * domain.y + patch[2].textureCoords * domain.z;
    Output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;
	
    const float heightMapSample = t_heightMap.SampleLevel(s_sampler, texCoords, 0).r * b_mapMultipler;

    Output.position.xyz += normal * heightMapSample;
	
    Output.position = mul(Output.position, modelViewProjection);

	return Output;
}
