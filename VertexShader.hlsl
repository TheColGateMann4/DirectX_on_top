cbuffer constBuffer
{
	matrix transform;
	//float time;
};

float4 main(float3 position : POSITION) : SV_POSITION
{
	//position.y = 3 * sin(position.x * time);
	return mul(float4(position, 1.0f), transform);
}