#include "VS_TransformConstBuffer.hlsli"
#include "VS_ShadowFunctions.hlsli"

struct DS_OUTPUT
{
    float3 viewPosition : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float2 textureCoords : TEXCOORD;
    float3 worldPosition : WORLDPOSITION;
    float3 worldNormal : WORLDNORMAL;
    float4 depthMapCoords : DEPTHTEXCOORD;
    float4 position : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 viewPosition : POSITION;
    float3 normal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float2 textureCoords : TEXCOORD;
    float3 worldPosition : WORLDPOSITION;
    float3 worldNormal : WORLDNORMAL;
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
    bool b_enableMultisampling;
    int b_sampleNumber;
};

cbuffer shadowBuffer : register(b2)
{
    matrix shadowViewProjection;
};

Texture2D t_heightMap : register(t0);
SamplerState s_sampler : register(s0);


#define NUM_CONTROL_POINTS 3

float3 orthogonal(float3 v) 
{
    return normalize(abs(v.x) > abs(v.z) ? float3(-v.y, v.x, 0.0)  : float3(0.0, -v.z, v.y));
}

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	Output.viewPosition = patch[0].viewPosition * domain.x + patch[1].viewPosition * domain.y + patch[2].viewPosition * domain.z;

	Output.viewNormal = patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;

	Output.viewBitangent = patch[0].viewBitangent * domain.x + patch[1].viewBitangent * domain.y + patch[2].viewBitangent * domain.z;

	Output.viewTangent = patch[0].viewTangent * domain.x + patch[1].viewTangent * domain.y + patch[2].viewTangent * domain.z;

	Output.textureCoords = patch[0].textureCoords * domain.x + patch[1].textureCoords * domain.y + patch[2].textureCoords * domain.z;

	Output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;

	Output.worldPosition = patch[0].worldPosition * domain.x + patch[1].worldPosition * domain.y + patch[2].worldPosition * domain.z;

	Output.worldNormal = patch[0].worldNormal * domain.x + patch[1].worldNormal * domain.y + patch[2].worldNormal * domain.z;


    float heightMapSample;

    if(b_enableMultisampling)
        heightMapSample = SampleNearPixels(t_heightMap, s_sampler, Output.textureCoords, b_sampleNumber).r * b_mapMultipler;
    else
        heightMapSample = t_heightMap.SampleLevel(s_sampler, Output.textureCoords, 0).r * b_mapMultipler;

    Output.position.xyz += Output.viewNormal * heightMapSample;

    Output.depthMapCoords = CalculateDepthTextureCoords(Output.position, model, shadowViewProjection);

    Output.position = mul(Output.position, modelViewProjection);

    Output.viewNormal = mul(Output.viewNormal, (float3x3)modelView);

	return Output;
}
