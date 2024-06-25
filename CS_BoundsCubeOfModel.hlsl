cbuffer verticesInfo : register(b0) 
{
    uint positionOffsetInStructureInFloatIndexes;
    uint structureSizeInFloatIndexes;
    uint targetResultID;
};

Buffer<float> verticeData : register(t0);
RWBuffer<float> result : register(u0);

#define RESULT_INDEX_MINUS_X 0
#define RESULT_INDEX_MINUS_Y 1
#define RESULT_INDEX_MINUS_Z 2
#define RESULT_INDEX_X 3
#define RESULT_INDEX_Y 4
#define RESULT_INDEX_Z 5

// this will be calculated only once per scene object

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint verticeDataSize, targetIDOffset = targetResultID * 6; // each model has 6 vertices
    verticeData.GetDimensions(verticeDataSize);

    for(int i = 0; i < verticeDataSize; i += structureSizeInFloatIndexes)
    {
        float3 vertexPosition;
        vertexPosition.x = verticeData[i + positionOffsetInStructureInFloatIndexes];
        vertexPosition.y = verticeData[i + positionOffsetInStructureInFloatIndexes + 1];
        vertexPosition.z = verticeData[i + positionOffsetInStructureInFloatIndexes + 2];

        if(vertexPosition.x < result[targetIDOffset + RESULT_INDEX_MINUS_X])
            result[targetIDOffset + RESULT_INDEX_MINUS_X] = vertexPosition.x;
        else if(vertexPosition.x > result[targetIDOffset + RESULT_INDEX_X])
            result[targetIDOffset + RESULT_INDEX_X] = vertexPosition.x;

        if(vertexPosition.y < result[targetIDOffset + RESULT_INDEX_MINUS_Y])
            result[targetIDOffset + RESULT_INDEX_MINUS_Y] = vertexPosition.y;
        else if(vertexPosition.y > result[targetIDOffset + RESULT_INDEX_Y])
            result[targetIDOffset + RESULT_INDEX_Y] = vertexPosition.y;

        if(vertexPosition.z < result[targetIDOffset + RESULT_INDEX_MINUS_Z])
            result[targetIDOffset + RESULT_INDEX_MINUS_Z] = vertexPosition.z;
        else if(vertexPosition.z > result[targetIDOffset + RESULT_INDEX_Z])
            result[targetIDOffset + RESULT_INDEX_Z] = vertexPosition.z;
    }
}