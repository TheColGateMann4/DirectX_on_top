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
    
    float padding[2];
}

Texture2D t_diffuseTexture : register(t0);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD) : SV_TARGET
{
    float4 diffuseSample = t_diffuseTexture.Sample(s_sampler, textureCoords);
    clip(diffuseSample.a < 1.0f ? -1.0f : 1.0f);
    
    normal = normalize(normal);
    
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