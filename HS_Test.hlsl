struct VS_CONTROL_POINT_OUTPUT
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

#define NUM_CONTROL_POINTS 3

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	Output.EdgeTessFactor[0] = 
		Output.EdgeTessFactor[1] = 
		Output.EdgeTessFactor[2] = 
		Output.InsideTessFactor = 15;

	return Output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	Output.viewPosition =  ip[i].viewPosition;
	Output.viewNormal =  ip[i].viewNormal;
	Output.viewTangent = ip[i].viewTangent;
	Output.viewBitangent = ip[i].viewBitangent;
	Output.textureCoords = ip[i].textureCoords;
	Output.depthMapCoords = ip[i].depthMapCoords;
	Output.position = ip[i].position;

	return Output;
}
