Buffer<float> modelBoxData : register(t0);
Buffer<float> cameraFrustumData : register(t1);

// if model with low id gets removed the id will be marked in this buffer as invalid, till some other model gets on its place
// if object doesn't have visible parts or we don't want to check, then we mark is as invalid as well
Buffer<uint> modelValidity : register(t2);

cbuffer SceneData : register(b0)
{
    uint numberOfModels;
}

RWBuffer<uint> result : register(u0);


#define CUBE_INDEX_MINUS_X 0
#define CUBE_INDEX_MINUS_Y 1
#define CUBE_INDEX_MINUS_Z 2
#define CUBE_INDEX_X 3
#define CUBE_INDEX_Y 4
#define CUBE_INDEX_Z 5

float3 GetCubeVerticeAtOffset(int3 verticeIndexes, uint modelIndex)
{
    uint modelIndexOffset = modelIndex * 6;

    return float3(modelBoxData[modelIndexOffset + verticeIndexes.x], modelBoxData[modelIndexOffset + verticeIndexes.y], modelBoxData[modelIndexOffset + verticeIndexes.z]);
}

float3 GetCubeVertice(uint verticeIndex, uint modelIndex)
{
    switch(verticeIndex)
    {
        case 0:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_MINUS_X, CUBE_INDEX_MINUS_Y, CUBE_INDEX_MINUS_Z), modelIndex);
        case 1:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_MINUS_X, CUBE_INDEX_Y, CUBE_INDEX_MINUS_Z), modelIndex);
        case 2:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_X, CUBE_INDEX_Y, CUBE_INDEX_MINUS_Z), modelIndex);
        case 3:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_X, CUBE_INDEX_MINUS_Y, CUBE_INDEX_MINUS_Z), modelIndex);
        case 4:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_MINUS_X, CUBE_INDEX_MINUS_Y, CUBE_INDEX_Z), modelIndex);
        case 5:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_MINUS_X, CUBE_INDEX_Y, CUBE_INDEX_Z), modelIndex);
        case 6:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_X, CUBE_INDEX_Y, CUBE_INDEX_Z), modelIndex);
        case 7:
            return GetCubeVerticeAtOffset( int3(CUBE_INDEX_X, CUBE_INDEX_MINUS_Y, CUBE_INDEX_Z), modelIndex);
        default:
            return float3(0.0f, 0.0f, 0.0f); // won't happen anyways
    }
}

float3 GetFrustumVertice(uint index)
{
    index *= 3;

    return float3(cameraFrustumData[index], cameraFrustumData[index + 1], cameraFrustumData[index + 2]);
}

// Checks if the ourPoint is between two points on slopes
// ! secondPoint has to be larger than firstPoint in the slope axis
// ! pointOnOtherSide is point going into - of axis
//
//              Imagine this is our view frustum:
//
//                                      ------* secondPoint
//                               ---X---
//                        -------   |  // we basicly get Y value where the point hit line
//        firstPoint *----          |  // between firstPoint and Second Point
//                   |              |
//               /   |              |
//              /    |              |
//       [Camera]    |              * ourPoint
//              \    |              |
//               \   |              |
//                   |              |
//  pointOnOtherSide *----          |
//                        -------   |
//                               ---X---
//                                      -------
//
//      *  represents point thats being taken as argument
//      X  represents point calculated, we use these to check if ourPoint is in wanted area
//
bool CheckForSlope(float3 firstPoint, float3 secondPoint, float3 pointOnOtherSide, float3 ourPoint, bool getY)
{
    float ourPointWantedAxis = getY ? ourPoint.y : ourPoint.x;    
    float firstPointWantedAxis = getY ? firstPoint.y : firstPoint.x;    
    float secondPointWantedAxis = getY ? secondPoint.y : secondPoint.x;
    float pointOnOtherSideWantedAxis = getY ? pointOnOtherSide.y : pointOnOtherSide.x;
    
    float difference = (abs(ourPoint.z) - abs(firstPoint.z)) / (abs(secondPoint.z) - abs(firstPoint.z));

    float offsetFromPoint = (secondPointWantedAxis - firstPointWantedAxis) * difference;

    float topPoint = firstPointWantedAxis + offsetFromPoint;
    float bottomPoint = pointOnOtherSideWantedAxis - offsetFromPoint;

    return ourPointWantedAxis < topPoint && ourPointWantedAxis > bottomPoint;
}

// when whole plan gets finished, this will be called:
// when camera moves: all objects per that camera view frustum
// when object moves: moved object per all camera view frustums

// looking forward to multiple threads :P
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

    for(int iModelID = 0; iModelID < numberOfModels; iModelID++)
    {
        if(modelValidity[iModelID] == 0)
            continue;

        //checking for vertices placed inside frustum
        for(int iCubeVertice = 0; iCubeVertice < 8; iCubeVertice++)
        {
            float3 verticePos = GetCubeVertice(iCubeVertice, iModelID);
            float3 nearZFrustumVertice = GetFrustumVertice(0);
            float3 farZFrustumVertice = GetFrustumVertice(4);

            if(verticePos.z < nearZFrustumVertice.z || verticePos.z > farZFrustumVertice.z)
                continue;

            if(!CheckForSlope(nearZFrustumVertice, farZFrustumVertice, GetFrustumVertice(1), verticePos, true))
                continue;

            if(!CheckForSlope(nearZFrustumVertice, farZFrustumVertice, GetFrustumVertice(3), verticePos, false))
                continue;

            result[iModelID] = 1;
        }
    }
}