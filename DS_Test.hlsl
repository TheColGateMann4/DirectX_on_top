#include "VS_TransformConstBuffer.hlsli"
#include "VS_ShadowFunctions.hlsli"

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
    float3 normal : NORMAL;
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

	const float heightMapSample = t_heightMap.SampleLevel(s_sampler, Output.textureCoords, 0).r * b_mapMultipler;
    Output.position.xyz += Output.viewNormal * heightMapSample;

    //getting displaced tangent bitangent and normal
//     {
//      int width, height;
//     
//         t_heightMap.GetDimensions(width, height);
//     
//         const float texelSize = 1.0f / width;
//     
//         const float3 tangent = orthogonal(Output.viewNormal);
//         const float3 bitangent = normalize(cross(Output.viewNormal, tangent));
//         const float3 neighbour1 = Output.position + tangent;
//         const float3 neighbour2 = Output.position + bitangent;
//     
//         const float2 neighbour1uv = heightMapSample + float2(-texelSize, 0);
//         const float2 neighbour2uv = heightMapSample  + float2(0, -texelSize);
//         const float3 displacedNeighbour1 = neighbour1 + Output.viewNormal * heightMapSample;
//         const float3 displacedNeighbour2 = neighbour2 + Output.viewNormal * heightMapSample;
//        
//         const float3 displacedTangent = displacedNeighbour1 - Output.position;
//         const float3 displacedBitangent = displacedNeighbour2 - Output.position;
//         
//         const float3 displacedNormal = normalize(cross(displacedTangent, displacedBitangent));
//     
//         Output.viewTangent = mul(displacedTangent, modelView);
//     
//         Output.viewBitangent = mul(displacedBitangent, modelView);
//     
//         Output.viewNormal = mul(displacedNormal, modelView);
//       }

    Output.depthMapCoords = CalculateDepthTextureCoords(Output.position, model, shadowViewProjection);

    Output.position = mul(Output.position, modelViewProjection);

    Output.viewNormal = mul(Output.viewNormal, (float3x3)modelView);

	return Output;
}
