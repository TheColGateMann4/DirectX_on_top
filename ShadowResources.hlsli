cbuffer shadowCameraData : register(b4)
{
    float c0;
    float c1;
    int pcf;
};

TextureCube t_depthMap : register(t6);
SamplerComparisonState s_depthComparisonSampler : register(s1);
SamplerState s_depthUsedSampler : register(s2);
SamplerState s_depthPointSampler : register(s3);