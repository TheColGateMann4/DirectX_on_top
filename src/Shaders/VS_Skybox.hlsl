cbuffer transformConstBuffer : register(b0)
{
    matrix cameraViewProjection;
}

struct VSOUT
{
    float3 worldPosition : POSITION;
    float4 position : SV_POSITION;
};

VSOUT main( float3 position : POSITION )
{
	VSOUT vsout;
    vsout.worldPosition = position;
    vsout.position = mul(float4(position, 0.0f), cameraViewProjection);
    
    vsout.position.z = vsout.position.w;
    
    return vsout;
}