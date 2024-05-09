#include "VS_TransformConstBuffer.hlsli"

struct DS_OUTPUT
{
    float3 viewPosition : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float2 textureCoords : TEXCOORD;
    float4 depthMapCoords : DEPTHTEXCOORD;
    float4 position : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 viewPosition : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float2 textureCoords : TEXCOORD;
    float4 depthMapCoords : DEPTHTEXCOORD;
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

	Output.viewPosition = float3(patch[0].viewPosition*domain.x+patch[1].viewPosition*domain.y+patch[2].viewPosition*domain.z);

	Output.viewNormal = float3(patch[0].viewNormal*domain.x+patch[1].viewNormal*domain.y+patch[2].viewNormal*domain.z);

	Output.viewTangent = float3(patch[0].viewTangent*domain.x+patch[1].viewTangent*domain.y+patch[2].viewTangent*domain.z);

	Output.viewBitangent = float3(patch[0].viewBitangent*domain.x+patch[1].viewBitangent*domain.y+patch[2].viewBitangent*domain.z);

	Output.textureCoords = float2(patch[0].textureCoords*domain.x+patch[1].textureCoords*domain.y+patch[2].textureCoords*domain.z);

	Output.depthMapCoords = float4(patch[0].depthMapCoords*domain.x+patch[1].depthMapCoords*domain.y+patch[2].depthMapCoords*domain.z);

	Output.position = float4(patch[0].position*domain.x+patch[1].position*domain.y+patch[2].position*domain.z);

	const float heightMapSample = t_heightMap.SampleLevel(s_sampler, Output.textureCoords, 0).r * b_mapMultipler;
    Output.position.xyz += Output.viewNormal * heightMapSample;

    Output.position = mul(Output.position, modelViewProjection);

	return Output;
}
