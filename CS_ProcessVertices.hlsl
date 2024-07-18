StructuredBuffer<matrix> objectTransform : register(t0);
Buffer<uint> modelValidity : register(t2);
RWBuffer<float> modelBoxData : register(u0);
RWBuffer<float> cameraFrustumData : register(u1);

cbuffer cameraData : register(b0)
{
    matrix cameraViewProjection;
};

cbuffer sceneData : register(b1)
{
    uint numberOfModels;
};

#define NUM_THREADS 100

[numthreads(NUM_THREADS, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint numPerThread = ceil(float(numberOfModels) / NUM_THREADS);
    for(uint iModel = 0; iModel < numPerThread; iModel++)
    {
        uint iModelID = numPerThread * DTid.x + iModel;

        if(iModelID > numberOfModels)
            return;

        if(modelValidity[iModelID] == 0)
            continue;

        uint verticeIndex = iModelID * 6;
        
        //each model has two vectors
        for(uint iVector = 0; iVector < 2; iVector++)
        {
            verticeIndex += iVector * 3;

            float3 modelBoxVertice;
    
            modelBoxVertice.x = modelBoxData[verticeIndex];
            modelBoxVertice.y = modelBoxData[verticeIndex + 1];
            modelBoxVertice.z = modelBoxData[verticeIndex + 2];

            modelBoxVertice = (float3)mul(float4(modelBoxVertice, 1.0f), objectTransform[iModelID]);

            modelBoxVertice = (float3)mul(float4(modelBoxVertice, 1.0f), cameraViewProjection);
    
            modelBoxData[verticeIndex] = modelBoxVertice.x;
            modelBoxData[verticeIndex + 1] = modelBoxVertice.y;
            modelBoxData[verticeIndex + 2] = modelBoxVertice.z;
        }
    }
}