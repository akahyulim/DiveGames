#include "Common.hlsl"

// �����δ� ������ ������ ���� �޶�����.
// MainVS��� �̸��� �����Ϸ��� ������ ������ Ȯ���� �� 
// ���� �Լ��� ȣ���ϵ��� �����ؾ� �Ѵ�.
/*
float4 MainVS(float4 input : POSITION0) : SV_Position
{
    float4 output;
    input.w = 1.0f;
    
    output = mul(input, cbModelVertex.world);
    output = mul(output, cbLightVertex.shadow);
        
    return output;
}
*/

// Ŭ���� ������ z������ depthstencilview�� ����ȴ�.
void MainVS(float4 iPos : POSITION0,
            out float4 oPos : SV_POSITION)
{
    oPos.w = 1.0f;
    
    oPos = mul(iPos, cbModelVertex.world);
    oPos = mul(oPos, cbLightVertex.shadow);
}

// rasterTeck�� ��� ������ �ؽ��İ� ���Ե� ������Ʈ���� �׸����� ���е��� ���̱� ���� srv�� ����Ѵ�.
// �̶��� ���ø� �� ���İ��� Ȯ���Ͽ� ���̰��� ������ �� ���θ� �Ǵ��ϴ� �ȼ� ���̴��� �ʿ��ϴ�.
// �� �ڵ�� ���� vs�� ȣȯ���� �ʴ´�.
// �׸��� ps������ sv_position�� ������ �� �����Ƿ� sv_target�� �´�.
// ��, dsv�� �ƴ϶� rtv�� ������ �ؾ��� �� ����.
void MainPS(float4 iPos : POSITION0,
            float2 iTexCoord: TEXCOORD,
            out float4 oColor : SV_TARGET)
{
    float depthValue;
    float4 diff = DiffuseMap.Sample(BaseSampler, iTexCoord);
    
    if(diff.a > 0.0f)
    {
        depthValue = iPos.z / iPos.w;
    }
    else
    {
        discard;
    }
    
    oColor = float4(depthValue, depthValue, depthValue, 1.0f);
}