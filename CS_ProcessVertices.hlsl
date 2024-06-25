StructuredBuffer<matrix> objectTransform : register(t0);
Buffer<uint> modelValidity : register(t2);
RWBuffer<float> modelBoxData : register(u0);
RWBuffer<float> cameraFrustumData : register(u1);

cbuffer cameraData : register(b0)
{
    matrix cameraViewProjection;
};

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint numModelsInBuffer;
    modelBoxData.GetDimensions(numModelsInBuffer);
    
    numModelsInBuffer /= 6;

    for(uint i = 0; i < numModelsInBuffer; i++)
    {
        if(modelValidity[i] == 0)
            continue;

        uint verticeIndex = i * 6;
        
        //each model has two vectors
        for(uint j = 0; j < 2; j++)
        {
            verticeIndex += j * 3;

            float3 modelBoxVertice;
    
            modelBoxVertice.x = modelBoxData[verticeIndex];
            modelBoxVertice.y = modelBoxData[verticeIndex + 1];
            modelBoxVertice.z = modelBoxData[verticeIndex + 2];

            modelBoxVertice = (float3)mul(float4(modelBoxVertice, 1.0f), objectTransform[i]);

            modelBoxVertice = (float3)mul(float4(modelBoxVertice, 1.0f), cameraViewProjection);
    
            modelBoxData[verticeIndex] = modelBoxVertice.x;
            modelBoxData[verticeIndex + 1] = modelBoxVertice.y;
            modelBoxData[verticeIndex + 2] = modelBoxVertice.z;
        }
    }
}