RWBuffer<uint> uav : register(u0);

[numthreads(20, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uav[DTid.x] = DTid.x;
}