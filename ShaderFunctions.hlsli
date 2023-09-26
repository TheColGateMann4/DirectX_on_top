
float3 GetNormalInViewSpace(
        const in float3 normal : NORMAL,
        const in float3 viewTangent : TANGENT,
        const in float3 viewBitangent : BITANGENT,
        const in float2 textureCoords : TEXCOORD,
        uniform SamplerState s_sampler,
        uniform Texture2D t_uvmapTexture,
        const in bool isRetardedBrickWallWithMessedUpNormals = false)
{
    //transform rotation in tangent space
    const float3x3 tangentTransform = { viewTangent, viewBitangent, normal };
        
    const float3 normalMapSample = t_uvmapTexture.Sample(s_sampler, textureCoords).rgb;
        
    float3 result;
    
    if(isRetardedBrickWallWithMessedUpNormals)
    {
        result.x = normalMapSample.x * 4.0f - 1.0f;
        result.y = normalMapSample.y * 4.0f - 1.0f;
        result.z = -normalMapSample.z * 2.0f + 1.0f;
    }
    else
    {
        result = normalMapSample * 2.0f - 1.0f;
        result.y = -result.y;
    }
    
    return normalize(mul(result, (float3x3) tangentTransform));
}

float GetAttenuation(
        const in float lengthOfVectorLength,
        uniform float b_attenuationConst,
        uniform float b_attenuationLinear,
        uniform float b_attenuationQuadratic)
{
    return 1 / (b_attenuationConst + b_attenuationLinear * lengthOfVectorLength + b_attenuationQuadratic * (lengthOfVectorLength * lengthOfVectorLength));
}

float3 GetDiffuse(
        const in float3 normal : NORMAL,
        const in float attenuation,
        const in float3 DirectionToLightSource,
        uniform float3 b_lightColor,
        uniform float b_diffuseIntensity)
{
    return b_lightColor * b_diffuseIntensity * attenuation * max(0.0f, dot(DirectionToLightSource, normal));
}

float3 GetSpecular(
        const in float3 positionRelativeToCamera : POSITION,
        const in float3 normal : NORMAL,
        const in float3 VectorLength,
        const in float attenuation,
        const in float specularPower,
        uniform float3 b_lightColor,
        uniform float b_diffuseIntensity)
{
    const float3 w = normal * dot(VectorLength, normal);
    const float3 r = w * 2.0f - VectorLength;
    
    return attenuation * (b_lightColor * b_diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(positionRelativeToCamera))), specularPower);
}