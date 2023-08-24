cbuffer lightBuffer : register(b0)
{
    float3 lightPosition;
    
    float3 ambient;
    float3 lightColor;
    
    float diffuseIntensity;
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuadratic;
};

cbuffer objectBuffer : register(b1)
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

cbuffer transformBuffer : register(b2)
{
    matrix modelView;
    matrix modelViewProjection;
};

Texture2D diffuseTexture : register(t0);
Texture2D uvmapTexture : register(t1);
SamplerState samplerr : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float2 textureCoords : TEXCOORD) : SV_TARGET
{
    if (normalMapEnabled)
    {
        const float3 normalMap = uvmapTexture.Sample(samplerr, textureCoords).rgb;
        
        normal.x = (normalMap.r - 0.2f) * 2.0f;
        normal.y = (normalMap.g - 0.2f) * 2.0f;
        normal.z = -normalMap.b;
        normal = mul(normal, (float3x3) modelView);
    }
    
    const float3 VectorLength = lightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = 1.0f / (attenuationConst + attenuationLinear * lengthOfVectorLength + attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
    
    const float3 diffuse = lightColor * diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    const float3 specular = attenuation * (lightColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), specularPower);
    
    return float4(saturate((diffuse + ambient) * diffuseTexture.Sample(samplerr, textureCoords).rgb + specular), 1.0f);
}