StructuredBuffer<matrix> objectTransform : register(t0);
Buffer<uint> modelValidity : register(t2);

RWBuffer<float> modelBoxData : register(u0);
// vertice minus
// vertice plus
// vertice side to plus - for later frustum calculations

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

        uint verticeIndex = iModelID * 9;
    
        // constructing our side vector here, before matrix multiplications
        modelBoxData[verticeIndex + 6] = 0;          // getting X value from minus-vertice
        modelBoxData[verticeIndex + 6 + 1] = 3 + 1;  // getting Y from plus-vertice
        modelBoxData[verticeIndex + 6 + 2] = 3 + 2;  // getting Z from plus-vertice

        for(uint iVector = 0; iVector < 3; iVector++)
        {
            verticeIndex += 3;

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