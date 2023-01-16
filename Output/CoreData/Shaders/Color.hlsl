// test shader
#include "ConstantBuffers.hlsl"


// urho는 전역? 변수같은 개별 타입별 cb와
// 기존과 같은 구조체형 cbuffer가 존재한다.
// 그런데 아직 구조체형을 gpu로 전달하는 방법을 찾지 못했다.

// 입력의 기본은 Geometry 타입으로 설정된 것으로 전달받아 구분하고
// =>(urho의 경우 Renderer에 조합 배열이 이미 정의 되어 있다.)
// 그외(DIFFMAP 등)에는 Tech의 Pass에 기술된 Define으로 전달받는다. 
void mainVS(
	float4 position : POSITION,
#ifdef VERTEXCOLOR
	float4 color : COLOR,
#endif
#ifdef DIFFMAP
	float2 texCoord : TEXCOORD0,
#endif
#ifdef VERTEXCOLOR
	out float4 oColor : COLOR,
#endif
#ifdef DIFFMAP
	out float2 oTexCoord : TEXCOORD0,
#endif
	out float4 oPosition : SV_POSITION
)
{
	position.w = 1.0f;

	oPosition = mul(position, cModel);
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
#ifdef VERTEXCOLOR
	float4 iColor : COLOR,
#endif
#ifdef DIFFMAP
	float2 iTexCoord : TEXCOORD0,
#endif
	float4 iPosition : SV_POSITION,
	out float4 oColor : SV_TARGET)		// urho는 OUTCOLOR0이라는 시맨틱을 사용하는데 나는 안된다. SV_TARGET인 듯 하다.
{
#ifdef VERTEXCOLOR
	oColor = iColor;
#else
	oColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
#endif

}