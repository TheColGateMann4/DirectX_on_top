cbuffer lightBuffer : register(b0)
{
    float3 lightPosition;
    
    float3 ambient;
    float3 diffuseColor;
    float3 lightColor;
    
    float diffuseIntensity;
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuadratic;
};

Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
SamplerState samplerr : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD) : SV_TARGET
{
    const float3 VectorLength = lightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = 1 / (attenuationConst + attenuationLinear * lengthOfVectorLength + attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
	
    const float3 diffuse = diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    const float4 specularSample = specularTexture.Sample(samplerr, textureCoords);
    const float3 specularColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    
    const float3 specular = attenuation * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), specularPower);
    
    return float4(saturate((diffuse + ambient) * diffuseTexture.Sample(samplerr, textureCoords).rgb + specular * specularColor), 1.0f);
}