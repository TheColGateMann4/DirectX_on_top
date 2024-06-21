RWBuffer<float> modelBoxData : register(u0);
RWBuffer<float> cameraFrustumData : register(u1);

cbuffer cameraData : register(b0)
{
    matrix cameraViewProjection;
};

// for now it processes data only for one model, but later it will process camerafrustum and modelBoxes of all models on scene
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{    
    {
        uint modelBoxDataSize;
        modelBoxData.GetDimensions(modelBoxDataSize);
    
        for(int i = 0; i < modelBoxDataSize; i += 3)
        {
            float3 modelBoxVertice;
    
            modelBoxVertice.x = modelBoxData[i];
            modelBoxVertice.y = modelBoxData[i + 1];
            modelBoxVertice.z = modelBoxData[i + 2];
    
            modelBoxVertice = (float3)mul(float4(modelBoxVertice, 1.0f), cameraViewProjection);
    
            modelBoxData[i] = modelBoxVertice.x;
            modelBoxData[i + 1] = modelBoxVertice.y;
            modelBoxData[i + 2] = modelBoxVertice.z;
        }
    }
}