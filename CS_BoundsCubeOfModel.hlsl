cbuffer verticesInfo : register(b0)
{
    uint positionOffsetInStructureInFloatIndexes;
    uint structureSizeInFloatIndexes;
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
    //hardcoding it for now to just get it done quick
    result[RESULT_INDEX_MINUS_X] = -1.0f;
    result[RESULT_INDEX_MINUS_Y] = -1.0f;
    result[RESULT_INDEX_MINUS_Z] = -1.0f;
    result[RESULT_INDEX_X] = 1.0f;
    result[RESULT_INDEX_Y] = 1.0f;
    result[RESULT_INDEX_Z] = 1.0f;
}