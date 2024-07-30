Buffer<float> modelBoxData : register(t0);
// vertice  x  y  z
// vertice -x  y  z
// vertice  x -y  z
// vertice -x -y  z
// vertice  x  y -z
// vertice -x  y -z
// vertice  x -y -z
// vertice -x -y -z

// mX = minus X
#define VERTICE_X_Y_Z    0
#define VERTICE_mX_Y_Z   1
#define VERTICE_X_mY_Z   2
#define VERTICE_mX_mY_Z  3
#define VERTICE_X_Y_mZ   4
#define VERTICE_mX_Y_mZ  5
#define VERTICE_X_mY_mZ  6
#define VERTICE_mX_mY_mZ 7

StructuredBuffer<float3> cameraFrustumData : register(t1);

// if model with low id gets removed the id will be marked in this buffer as invalid, till some other model gets on its place
// if object doesn't have visible parts or we don't want to check, then we mark is as invalid as well
Buffer<uint> modelValidity : register(t2);

cbuffer SceneData : register(b0)
{
    uint numberOfModels;
};

RWBuffer<uint> result : register(u0);

float3 GetCubeVertice(uint verticeIndex, uint modelIndex)
{
    uint modelIndexOffset = modelIndex * 3 * 8 + verticeIndex * 3;

    return float3(modelBoxData[modelIndexOffset], modelBoxData[modelIndexOffset + 1], modelBoxData[modelIndexOffset + 2]);
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

        bool passedDirectVisibleCheck = false;

        //checking for vertices placed inside frustum
        for(uint iCubeVertice = 0; iCubeVertice < 8; iCubeVertice++)
        {
            float3 verticePos = GetCubeVertice(iCubeVertice, iModelID);
            float3 nearZFrustumVertice = cameraFrustumData[0];
            float3 farZFrustumVertice = cameraFrustumData[4];
    
            if(verticePos.z < nearZFrustumVertice.z || verticePos.z > farZFrustumVertice.z)
                continue;
    
            if(!CheckForSlope(nearZFrustumVertice, farZFrustumVertice, cameraFrustumData[1].y, verticePos, true))
                continue;
    
            if(!CheckForSlope(nearZFrustumVertice, farZFrustumVertice, cameraFrustumData[3].x, verticePos, false))
                continue;
    
            result[iModelID] = 1;
            passedDirectVisibleCheck = true;
            break;
        }
    
        // if object is already marked as visible we don't need to check it in more complicated ways
        if(passedDirectVisibleCheck)
            continue;
    
    
//        //checking for vertices that are around frustum, and we don't see them, but their middle part can be seen
//        float3 cubeMinusVerticePos = GetCubeVertice(7, iModelID);
//        float3 cubePlusVerticePos = GetCubeVertice(0, iModelID);
//    
//        float3 nearZFrustumVertice = cameraFrustumData[0];
//        float3 farZFrustumVertice = cameraFrustumData[4];
//    
//        for(uint iFrustumVertice = 0; iFrustumVertice < 8; iFrustumVertice++)
//        {
//            float3 frustumVerticePos = cameraFrustumData[iFrustumVertice];
//        
//            // the point of this is, when object is infront of camera it has to be on at least two axels to be in frustum, thats why its okay if its not on one axis
//            bool failedOnce = false;
//                
//    
//            // checking Z axis
//            {
//                if(!CheckIfObjectsAxisOverlapsWithFrustum(cubePlusVerticePos.z, cubeMinusVerticePos.z, frustumVerticePos.z, farZFrustumVertice.z, nearZFrustumVertice.z))
//                    failedOnce = true;
//            }
//            
//    
//            // top and bottom point on current slope at given Z depth
//            float topPoint, bottomPoint;
//    
//    
//            // checking X axis, we need to account for slope on view frustum
//            {                    
//                GetSlopeValues(nearZFrustumVertice, farZFrustumVertice, cameraFrustumData[3].y, cubePlusVerticePos, false, topPoint, bottomPoint);
//                
//                if(!CheckIfObjectsAxisOverlapsWithFrustum(cubePlusVerticePos.x, cubeMinusVerticePos.x, frustumVerticePos.x, topPoint, bottomPoint))
//                    if(failedOnce)
//                    {
//                        continue;
//                    }
//                    else
//                        failedOnce = true;
//            }
//          
//    
//            // checking Y axis, we need to account for slope on view frustum as well
//            {
//                GetSlopeValues(nearZFrustumVertice, farZFrustumVertice, cameraFrustumData[1].x, cubePlusVerticePos, true, topPoint, bottomPoint);
//    
//                if(!CheckIfObjectsAxisOverlapsWithFrustum(cubePlusVerticePos.y, cubeMinusVerticePos.y, frustumVerticePos.y, topPoint, bottomPoint))
//                    if(failedOnce)
//                        continue;
//            }
//    
//            result[iModelID] = 1;
//            break;
//        }
    }
}