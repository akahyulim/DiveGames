#include "ConstantBuffers.hlsl"
#include "Samplers.hlsl"

void mainVS(
	float4 iPosition : POSITION,
#ifdef DIFFMAP
	float2 iTexCoord : TEXCOORD0,
#endif
#ifdef VERTEXCOLOR
	float4 iColor : COLOR,
#endif
#ifdef DIFFMAP
	float2 oTexCoord : TEXCOORD0,
#endif
#ifdef VERTEXCOLOR
	out float4 oColor : COLOR,
#endif
	out float4 oPosition : SV_POSITION
)
{
	iPosition.w = 1.0f;

	oPosition = mul(iPosition, cModel);
	oPosition = mul(oPosition, viewMatrix);
	oPosition = mul(oPosition, projectionMatrix);

#ifdef DIFFMAP
	oTexCoord = iTexCoord;
#endif

#ifdef VERTEXCOLOR
	oColor = iColor;
#endif

}

void mainPS(
#ifdef DIFMAP
	float2 iTexCoord : TEXCOORD0,
#endif
#ifdef VERTEXCOLOR
	float4 iColor : COLOR,
#endif
	out float4 oColor : SV_TARGET)
{
	// urho는 cMatDiffColor이라는게 따로 존재한다.
	float4 diffColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef VERTEXCOLOR
	diffColor *= iColor;
#endif

#if(!defined(DIFFMAP))
	oColor = diffColor;
#endif

#ifdef DIFFMAP
	// 텍스쳐와 샘플러가 필요하다.
	// 둘 다 samplers.hlsl에 있다.

	#ifdef ALPHAMASK
		// 샘플링한 색의 a가 0.5보다 작으면 discard;
		// 생각했던 것과는 다르다...
	#endif
#endif
}