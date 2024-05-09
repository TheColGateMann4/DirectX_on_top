#include "VS_ShadowFunctions.hlsli"
#include "VS_TransformConstBuffer.hlsli"

cbuffer shadowBuffer : register(b1)
{
    matrix shadowViewProjection;
};

struct VSOUT
{
    float3 viewPosition : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float2 textureCoords : TEXCOORD;
    float4 depthMapCoords : DEPTHTEXCOORD;
    float4 position : SV_POSITION;
};

VSOUT main(float3 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 textureCoords : TEXCOORD)
{
    VSOUT vsout;
    vsout.viewPosition = (float3) mul(float4(position, 1.0f), modelView);
    vsout.viewNormal = mul(normal, (float3x3) modelView);
    vsout.viewTangent = mul(tangent, (float3x3) modelView);
    vsout.viewBitangent = mul(bitangent, (float3x3)modelView);
    vsout.textureCoords = textureCoords;
    vsout.position = float4(position, 1.0f);
    vsout.depthMapCoords = CalculateDepthTextureCoords(vsout.position, model, shadowViewProjection);

    return vsout;
}