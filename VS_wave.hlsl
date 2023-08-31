cbuffer constBuffer
{
	matrix transform;
	float currtime;
};

float4 main(float3 position : POSITION) : SV_POSITION
{
	position.y = sin(position.x * 1.5 + currtime * 0.000001) + position.y;
	return mul(float4(position, 1.0f), transform);
}