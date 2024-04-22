cbuffer shadowSettingsBuffer : register(b4)
{
    int PCF_level;
    float bias;
    bool hardwarePCF;
    bool bilinear;
    bool circleFilter;
    float radius;
};

TextureCube t_depthMap : register(t3);
SamplerState s_depthSampler : register(s1);
SamplerComparisonState s_depthComparisonSampler : register(s2);