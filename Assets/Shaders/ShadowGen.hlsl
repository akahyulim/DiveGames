#include "Common.hlsl"

// �����δ� ������ ������ ���� �޶�����.
// MainVS��� �̸��� �����Ϸ��� ������ ������ Ȯ���� �� 
// ���� �Լ��� ȣ���ϵ��� �����ؾ� �Ѵ�.
float4 MainVS(float4 input : POSITION0) : SV_Position
{
    float4 output;
    input.w = 1.0f;
    
    output = mul(input, cbFrameVertex.world);
    output = mul(output, cbLightVertex.shadow);
        
    return output;
}