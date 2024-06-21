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

// for now it works for single modelBoxData, but later we will have multiple thread groups where each one is checking separate model from array
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    for(int iCubeVertice = 0; iCubeVertice < 8; iCubeVertice++)
    {
        float3 verticePos = GetCubeVertice(iCubeVertice);
        if(verticePos.z < GetFrustumVertice(0).z || verticePos.z > GetFrustumVertice(4).z)
            continue;

        if(!CheckForSlope(GetFrustumVertice(0), GetFrustumVertice(4), GetFrustumVertice(1), verticePos, true))
            continue;

        if(!CheckForSlope(GetFrustumVertice(0), GetFrustumVertice(4), GetFrustumVertice(3), verticePos, false))
            continue;

        result[0] = 1;
        return;
    }
}