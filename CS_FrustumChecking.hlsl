Buffer<float> modelBoxData : register(t0);
Buffer<float> cameraFrustumData : register(t1);

RWBuffer<uint> result : register(u0);

#define CUBE_INDEX_MINUS_X 0
#define CUBE_INDEX_MINUS_Y 1
#define CUBE_INDEX_MINUS_Z 2
#define CUBE_INDEX_X 3
#define CUBE_INDEX_Y 4
#define CUBE_INDEX_Z 5

float3 GetCubeVertice(uint index)
{
    switch(index)
    {
        case 0:
            return float3(modelBoxData[CUBE_INDEX_MINUS_X], modelBoxData[CUBE_INDEX_MINUS_Y], modelBoxData[CUBE_INDEX_MINUS_Z]);
        case 1:
            return float3(modelBoxData[CUBE_INDEX_MINUS_X], modelBoxData[CUBE_INDEX_Y], modelBoxData[CUBE_INDEX_MINUS_Z]);
        case 2:
            return float3(modelBoxData[CUBE_INDEX_X], modelBoxData[CUBE_INDEX_Y], modelBoxData[CUBE_INDEX_MINUS_Z]);
        case 3:
            return float3(modelBoxData[CUBE_INDEX_X], modelBoxData[CUBE_INDEX_MINUS_Y], modelBoxData[CUBE_INDEX_MINUS_Z]);
        case 4:
            return float3(modelBoxData[CUBE_INDEX_MINUS_X], modelBoxData[CUBE_INDEX_MINUS_Y], modelBoxData[CUBE_INDEX_Z]);
        case 5:
            return float3(modelBoxData[CUBE_INDEX_MINUS_X], modelBoxData[CUBE_INDEX_Y], modelBoxData[CUBE_INDEX_Z]);
        case 6:
            return float3(modelBoxData[CUBE_INDEX_X], modelBoxData[CUBE_INDEX_Y], modelBoxData[CUBE_INDEX_Z]);
        case 7:
            return float3(modelBoxData[CUBE_INDEX_X], modelBoxData[CUBE_INDEX_MINUS_Y], modelBoxData[CUBE_INDEX_Z]);
        default:
            return float3(0.0f, 0.0f, 0.0f); // won't happen anyways
    }
}

float3 GetFrustumVertice(uint index)
{
    index *= 3;

    return float3(cameraFrustumData[index], cameraFrustumData[index + 1], cameraFrustumData[index + 2]);
}

// when whole plan gets finished, this will be called:
// when camera moves: all objects per that camera view frustum
// when object moves: moved object per all camera view frustums

// for now it works for single modelBoxData, but later we will have multiple thread groups where each one is checking separate model from array
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    for(int iCubeVertice = 0; iCubeVertice < 8; iCubeVertice++)
    {
        float3 verticePos = GetCubeVertice(iCubeVertice);

        if(verticePos.y < GetFrustumVertice(5).y || verticePos.y > GetFrustumVertice(4).y)
            continue;

        if(verticePos.z < GetFrustumVertice(0).z || verticePos.z > GetFrustumVertice(4).z)
            continue;

        if(verticePos.x < GetFrustumVertice(6).x || verticePos.x > GetFrustumVertice(5).x) // its not cube so we need to account for slope
            continue;

        result[0] = 1;
        return;
    }   
    result[0] = 0;
}