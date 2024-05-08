struct DS_OUTPUT
{
    float3 positionRelativeToCamera : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 positionRelativeToCamera : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor;
	float InsideTessFactor			: SV_InsideTessFactor;
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
		patch[0].position*domain.x+patch[1].position*domain.y+patch[2].position*domain.z);

	Output.normal = float3(
		patch[0].normal*domain.x+patch[1].normal*domain.y+patch[2].normal*domain.z);

	Output.positionRelativeToCamera = float3(
		patch[0].positionRelativeToCamera*domain.x+patch[1].positionRelativeToCamera*domain.y+patch[2].positionRelativeToCamera*domain.z);

	return Output;
}
