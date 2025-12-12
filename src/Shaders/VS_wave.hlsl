#include "VS_TransformConstBuffer.hlsli"

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

float4 main(float3 position : POSITION) : SV_POSITION
{
	position.y = (sin(position.x * stepness+ time * speed) - 0.5f) * scale + position.y;
	return mul(float4(position, 1.0f), modelViewProjection);
}