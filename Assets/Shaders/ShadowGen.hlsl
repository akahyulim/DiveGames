#include "Common.hlsl"

// 실제로는 광원의 종류에 따라 달라진다.
// MainVS라는 이름은 유지하려면 광원의 종류를 확인한 후 
// 개별 함수를 호출하도록 수정해야 한다.
float4 MainVS(float4 input : POSITION0) : SV_Position
{
    float4 output;
    input.w = 1.0f;
    
    output = mul(input, cbFrameVertex.world);
    output = mul(output, cbLightVertex.shadow);
        
    return output;
}