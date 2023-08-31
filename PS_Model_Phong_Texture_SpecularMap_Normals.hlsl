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
    bool b_normalMapEnabled;
    bool b_normalMapHasAlpha;
    bool b_specularMapEnable;
    float b_specularPower;
    float3 b_specularColor;
    float b_specularMapWeight;
};

Texture2D t_diffuseTexture : register(t0);
Texture2D t_specularTexture : register(t1);
Texture2D t_uvmapTexture : register(t2);
SamplerState s_sampler : register(s0);

float4 main(float3 positionRelativeToCamera : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 textureCoords : TEXCOORD) : SV_TARGET
{
    if (b_normalMapEnabled)
    {
        //transform rotation in tangent space
        const float3x3 tangentTransform =
        {
            normalize(tangent),
            normalize(bitangent),
            normalize(normal)
        };
        
        const float3 normalMap = t_uvmapTexture.Sample(s_sampler, textureCoords).rgb;
        
        normal.x = normalMap.r * 2.0f - 1.0f;
        normal.y = normalMap.g * 2.0f - 1.0f;
        normal.z = normalMap.b;
        normal = mul(normal, (float3x3) tangentTransform);
    }
    
    const float3 VectorLength = b_lightPosition - positionRelativeToCamera;
    const float lengthOfVectorLength = length(VectorLength);
    const float3 DirectionToLightSource = VectorLength / lengthOfVectorLength;
    
    const float attenuation = 1 / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
	
    const float3 diffuse = b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
    
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    const float4 specularSample = t_specularTexture.Sample(s_sampler, textureCoords);
    const float3 specularColor = specularSample.rgb;
    float specularPower = 0;
    
    if (b_normalMapHasAlpha)
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    else
        specularPower = b_specularPower;
    
    const float3 specular = attenuation * (b_lightColor * b_diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), specularPower);
    
    return float4(saturate((diffuse + b_ambient) * t_diffuseTexture.Sample(s_sampler, textureCoords).rgb + specular * specularColor), 1.0f);
}