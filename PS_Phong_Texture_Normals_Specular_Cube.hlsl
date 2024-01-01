#include "ShaderFunctions.hlsli"

cbuffer lightBuffer : register(b0)
{
    float3 b_lightPosition;
    
    float3 b_ambient;
    float3 b_lightColor;
    
    float b_diffuseIntensity;
    float b_attenuationConst;
    float b_attenuationLinear;
    float b_attenuationQuadratic;
};

cbuffer objectBuffer : register(b1)
{
    float b_specularIntensity;
    float b_specularPower;
    bool b_normalMapEnabled;
    float b_normalMapWeight;
};

cbuffer transformBuffer : register(b2)
{
    matrix b_modelView;
    matrix b_modelViewProjection;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_uvmapTexture : register(t1);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD) : SV_TARGET
{
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);
    
    if (b_normalMapEnabled)
    {
        const float3 normalOfFace = normal;
        const float3 normalMap = t_uvmapTexture.Sample(s_sampler, textureCoords).rgb;
        
        normal.xy = (normalMap.xy * 2.0f) - 1.0f;
        normal.z = -normalMap.z * 2;
        
        float3 axis = float3(normalOfFace.y, normalOfFace.z, normalOfFace.x);
        float angle = 3.14159265f;
        
        if (normalOfFace.z < 0)
        {
            angle = 2 * angle;
        }
        else if (normalOfFace.y != 0)
        {
            angle = 1.5f * angle;
        }
        else if (normalOfFace.x != 0)
        {
            axis = float3(0, 1, 0);
            
            if (normalOfFace.x < 0)
                angle = 1.5 * angle;
            else
                angle = 0.5 * angle;
        }
       
        
        float3x3 rotationMatrix = AngleAxis3x3(angle, axis);
        
        normal.z = mul(normal.z, b_normalMapWeight);
        normal = mul(normal, rotationMatrix);
    }
    
    normal = normalize(mul(normal, (float3x3) b_modelView));
    
    const float3 VectorLength = b_lightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = 1.0f / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
	
    const float3 diffuse = b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    const float3 specular = attenuation * (b_lightColor * b_diffuseIntensity) * b_specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), b_specularPower);
    
    return float4(saturate((diffuse + b_ambient) * diffuseSample.rgb + specular), diffuseSample.a);
}