struct VSOUT
{
	float2 texturePos : TEXCOORD;
	float4 position : SV_POSITION;
};

VSOUT main( float2 position : POSITION2D )
{
	VSOUT vsout;
	vsout.texturePos = float2((position.x + 1.0f) / 2.0f, -(position.y - 1.0f) / 2.0f);
	vsout.position = float4(position, 0.0f, 1.0f);
	return vsout;
}