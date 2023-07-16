cbuffer lightBuffer : register(b0)
{
    float3 lightPosition;
    
    float3 ambient;
    float3 diffuseColor;
    
    float diffuseIntensity;
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuadratic;
};

cbuffer objectBuffer : register(b1)
{
    float3 materialColor;
    
    float specularIntensity;
    float specularPower;
}

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL) : SV_TARGET
{
    const float3 VectorLength = lightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = 1 / (attenuationConst + attenuationLinear * DirectionToLightSource + attenuationQuadratic * (lengthOfVectorLength * DirectionToLightSource));
	
    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    const float3 specular = attenuation * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), specularPower);
    
    return float4(saturate(diffuse + ambient + specular) * materialColor, 1.0f);
}