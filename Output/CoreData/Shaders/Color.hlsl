// test shader
#include "ConstantBuffers.hlsl"


// urho는 전역? 변수같은 개별 타입별 cb와
// 기존과 같은 구조체형 cbubuffer가 존재한다.
// 그런데 아직 구조체형을 gpu로 전달하는 방법을 찾지 못했다.

// position은 기본이고
// 이후 인자부턴 파라미터로 확인한다.
void mainVS(
	float4 position : POSITION,
#ifdef DIFFMAP
	float2 texCoord : TEXCOORD0,
#endif
#ifdef VERTEXCOLOR
	float4 color : COLOR,
#endif
	out float4 oPosition : SV_POSITION,
#ifdef DIFFMAP
	float2 oTexCoord : TEXCOORD0, 
#endif
	out float4 oColor : COLOR				
)
{
	position.w = 1.0f;

	oPosition = mul(position, worldMatrix);
	oPosition = mul(oPosition, viewMatrix);
	oPosition = mul(oPosition, projectionMatrix);

#ifdef DIFFMAP
	oTexCoord = texCoord;
#endif

#ifdef VERTEXCOLOR
	oColor = color;
#else
	oColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
#endif

}

void mainPS(
	float4 position : SV_POSITION,
#ifdef DIFMAP
	float2 texCoord : TEXCOORD0,
#endif
	float4 color : COLOR,				// mainVS의 out과 순서가 같아야 한다.
	out float4 oColor : SV_TARGET)
{
	oColor = color;
}