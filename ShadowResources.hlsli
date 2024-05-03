cbuffer shadowCameraData : register(b4)
{
    float c0;
    float c1;
};

TextureCube t_depthMap : register(t3);
SamplerComparisonState s_depthComparisonSampler : register(s1);