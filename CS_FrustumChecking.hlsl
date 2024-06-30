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

// Gets values on both slopes on level of ourPoint
// ! secondPoint has to be larger than firstPoint in the slope axis
// ! pointOnOtherSide is point going into - of axis
//
//              Imagine this is our view frustum:
//
//                                      ------* secondPoint
//                               ---X---
//                        -------   |
//        firstPoint *----          |
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
//      X  represents the values that will be calculated, topPoint and bottomPoint
//
void GetSlopeValues(float3 firstPoint, float3 secondPoint, float pointOnOtherSide, float3 ourPoint, bool getY, out float topPoint, out float bottomPoint)
{
    float firstPointWantedAxis = getY ? firstPoint.y : firstPoint.x;    
    float secondPointWantedAxis = getY ? secondPoint.y : secondPoint.x;
    
    float difference = (ourPoint.z - firstPoint.z) / (secondPoint.z - firstPoint.z);

    float offsetFromPoint = (secondPointWantedAxis - firstPointWantedAxis) * difference;

    topPoint = firstPointWantedAxis + offsetFromPoint;
    bottomPoint = pointOnOtherSide - offsetFromPoint;
}

// Checks if our point is in between sloped 
bool CheckForSlope(float3 firstPoint, float3 secondPoint, float pointOnOtherSide, float3 ourPoint, bool getY)
{
    float ourPointWantedAxis = getY ? ourPoint.y : ourPoint.x;
    float topPoint, bottomPoint;

    GetSlopeValues(firstPoint, secondPoint, pointOnOtherSide, ourPoint, getY, topPoint, bottomPoint);

    return ourPointWantedAxis < topPoint && ourPointWantedAxis > bottomPoint;
}

bool CheckIfObjectsAxisOverlapsWithFrustum(float cubePlusVerticePosAxis, float cubeMinusVerticePosAxis, float frustumVerticePosAxis, float farZFrustumVerticeAxis, float nearZFrustumVerticeAxis)
{
    // "first test" checks if the cube vertices are in this scheme:
    //
    //      *                 *                 *
    // cubeVertice      FrustumVertice     cubeVertice

    // "second test" checks if the cube vertices are in frustum range, for example Z axis
    //  View frustum Z-axis from camera: 
    //
    //      [Camera] nearZ<------------------------------------------->FarZ
    //                                  *
    //                      point in correct position

    // normal first test
    if(cubePlusVerticePosAxis > frustumVerticePosAxis && frustumVerticePosAxis > cubeMinusVerticePosAxis)
        return true;

    // reversed first test
    if(cubePlusVerticePosAxis < frustumVerticePosAxis && frustumVerticePosAxis < cubeMinusVerticePosAxis)
        return true;

    // normal second test
    if(cubePlusVerticePosAxis > nearZFrustumVerticeAxis && farZFrustumVerticeAxis > cubeMinusVerticePosAxis)
        return true;

    // reversed second test
    if(cubePlusVerticePosAxis < nearZFrustumVerticeAxis && farZFrustumVerticeAxis < cubeMinusVerticePosAxis)
        return true;

    return false;
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

        bool passedDirectVisibleCheck = false;

        //checking for vertices placed inside frustum
        for(int iCubeVertice = 0; iCubeVertice < 8; iCubeVertice++)
        {
            float3 verticePos = GetCubeVertice(iCubeVertice, iModelID);
            float3 nearZFrustumVertice = GetFrustumVertice(0);
            float3 farZFrustumVertice = GetFrustumVertice(4);

            if(verticePos.z < nearZFrustumVertice.z || verticePos.z > farZFrustumVertice.z)
                continue;

            if(!CheckForSlope(nearZFrustumVertice, farZFrustumVertice, GetFrustumVertice(1).y, verticePos, true))
                continue;

            if(!CheckForSlope(nearZFrustumVertice, farZFrustumVertice, GetFrustumVertice(3).x, verticePos, false))
                continue;

            result[iModelID] = 1;
            passedDirectVisibleCheck = true;
            break;
        }

        // if object is already marked as visible we don't need to check it in more complicated ways
        if(passedDirectVisibleCheck)
            continue;


        //checking for vertices that are around frustum, and we don't see them, but their middle part can be seen
        float3 cubeMinusVerticePos = GetCubeVertice(0, iModelID);
        float3 cubePlusVerticePos = GetCubeVertice(6, iModelID);

        float3 nearZFrustumVertice = GetFrustumVertice(0);
        float3 farZFrustumVertice = GetFrustumVertice(4);

        for(int iFrustumVertice = 0; iFrustumVertice < 8; iFrustumVertice++)
        {
            float3 frustumVerticePos = GetFrustumVertice(iFrustumVertice);
        
            // the point of this is, when object is infront of camera it has to be on at least two axels to be in frustum, thats why its okay if its not on one axis
            bool failedOnce = false;
                

            // checking Z axis
            {
                if(!CheckIfObjectsAxisOverlapsWithFrustum(cubePlusVerticePos.z, cubeMinusVerticePos.z, frustumVerticePos.z, farZFrustumVertice.z, nearZFrustumVertice.z))
                    failedOnce = true;
            }
            

            // top and bottom point on current slope at given Z depth
            float topPoint, bottomPoint;


            // checking X axis, we need to account for slope on view frustum
            {                    
                GetSlopeValues(nearZFrustumVertice, farZFrustumVertice, GetFrustumVertice(3).y, cubePlusVerticePos, false, topPoint, bottomPoint);
                
                if(!CheckIfObjectsAxisOverlapsWithFrustum(cubePlusVerticePos.x, cubeMinusVerticePos.x, frustumVerticePos.x, topPoint, bottomPoint))
                    if(failedOnce)
                    {
                        continue;
                    }
                    else
                        failedOnce = true;
            }
          

            // checking Y axis, we need to account for slope on view frustum as well
            {
                GetSlopeValues(nearZFrustumVertice, farZFrustumVertice, GetFrustumVertice(1).x, cubePlusVerticePos, true, topPoint, bottomPoint);

                if(!CheckIfObjectsAxisOverlapsWithFrustum(cubePlusVerticePos.y, cubeMinusVerticePos.y, frustumVerticePos.y, topPoint, bottomPoint))
                    if(failedOnce)
                        continue;
            }

            result[iModelID] = 1;
            break;
        }
    }
}