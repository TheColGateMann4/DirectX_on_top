cbuffer shadowCameraData : register(b4)
{
    float c0;
    float c1;
    int pcf;
};

TextureCube t_depthMap : register(t5);
SamplerComparisonState s_depthComparisonSampler : register(s1);