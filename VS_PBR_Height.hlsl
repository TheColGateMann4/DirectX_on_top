#include "VS_ShadowFunctions.hlsli"
#include "VS_TransformConstBuffer.hlsli"

cbuffer shadowBuffer : register(b1)
{
    matrix shadowViewProjection;
};

struct VSOUT
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

VSOUT main(float3 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 textureCoords : TEXCOORD)
{
    VSOUT vsout;
    vsout.viewPosition = (float3) mul(float4(position, 1.0f), modelView);
    vsout.normal = normal;
    vsout.viewTangent = mul(tangent, (float3x3) modelView);
    vsout.viewBitangent = mul(bitangent, (float3x3)modelView);
    vsout.textureCoords = textureCoords;
    vsout.depthMapCoords = CalculateDepthTextureCoords(position, model, shadowViewProjection); 
    vsout.worldPosition = mul(position, (float3x3)model);
    vsout.worldNormal = mul(normal, (float3x3)model);
    vsout.position = float4(position, 1.0f);

    return vsout;
}