#include "Common.hlsl"

// 실제로는 광원의 종류에 따라 달라진다.
// MainVS라는 이름은 유지하려면 광원의 종류를 확인한 후 
// 개별 함수를 호출하도록 수정해야 한다.
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

// 클리핑 공간의 z값으로 depthstencilview에 저장된다.
void MainVS(float4 iPos : POSITION0,
            out float4 oPos : SV_POSITION)
{
    oPos.w = 1.0f;
    
    oPos = mul(iPos, cbModelVertex.world);
    oPos = mul(oPos, cbLightVertex.shadow);
}

// rasterTeck의 경우 투명한 텍스쳐가 포함된 오브젝트에서 그림자의 정밀도를 높이기 위해 srv도 사용한다.
// 이때는 샘플링 후 알파값을 확인하여 깊이값을 저장할 지 여부를 판단하는 픽셀 셰이더가 필요하다.
// 이 코드는 위의 vs와 호환되지 않는다.
// 그리고 ps에서는 sv_position을 리턴할 수 없으므로 sv_target이 맞다.
// 즉, dsv가 아니라 rtv로 관리를 해야할 것 같다.
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