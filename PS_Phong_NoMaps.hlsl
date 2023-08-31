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
    float4 b_materialColor;
    float4 b_specularColor;
    float b_specularPower;
};

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL) : SV_TARGET
{
    const float3 VectorLength = b_lightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = 1.0f / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
    
    const float3 diffuse = b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    const float3 specular = attenuation * (b_lightColor * b_diffuseIntensity) * b_specularColor.rgb * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), b_specularPower);
    
    return float4(saturate((diffuse + b_ambient) * b_materialColor.rgb + specular), 1.0f);
}