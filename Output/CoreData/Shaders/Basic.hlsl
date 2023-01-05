#include "ConstantBuffers.hlsl"
#include "Samplers.hlsl"

void mainVS(
	float4 iPosition : POSITION,
#ifdef VERTEXCOLOR
	float4 iColor : COLOR,
#endif
#ifdef DIFFMAP
	float2 iTexCoord : TEXCOORD0,
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
	iPosition.w = 1.0f;

	oPosition = mul(iPosition, cModel);
	oPosition = mul(oPosition, viewMatrix);
	oPosition = mul(oPosition, projectionMatrix);

#ifdef VERTEXCOLOR
	oColor = iColor;
#endif

#ifdef DIFFMAP
	oTexCoord = iTexCoord;
#endif

}

float4 texLoad(float2 texCoord)
{
	uint width, height;
	tDiffMap.GetDimensions(width, height);
	float3 texCoord3 = float3(texCoord.x * width, texCoord.y * height, 0);
	return tDiffMap.Load(texCoord3);
}

void mainPS(
#ifdef VERTEXCOLOR
	float4 iColor : COLOR,
#endif
#ifdef DIFFMAP
	float2 iTexCoord : TEXCOORD0,
#endif
	float4 iPosition : SV_POSITION,
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
	float4 diffInput = texLoad(iTexCoord);

	#ifdef ALPHAMASK
		if (diffInput.a < 0.5)
			discard;
	#endif
	oColor = diffColor * diffInput;
#endif
}