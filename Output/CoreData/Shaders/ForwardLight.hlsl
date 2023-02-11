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
#if !defined(BILLBOARD)
	float3 iNormal : NORMAL,
#endif
#ifdef VERTEXCOLOR
	out float4 oColor : COLOR0,
#endif
#ifdef DIFFMAP
	out float2 oTexCoord : TEXCOORD0,
#endif
#if !defined(BILLBOARD)
	out float3 oNormal : NORMAL,
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

#if !defined(BILLBOARD)
	oNormal = mul(iNormal, (float3x3)cModel);
	oNormal = normalize(oNormal);
#endif
}

float4 loadTex(float2 texCoord)
{
	uint width, height;
	DiffMapTex.GetDimensions(width, height);
	float3 texCoord3 = float3(texCoord.x * width, texCoord.y * height, 0);
	return DiffMapTex.Load(texCoord3);
}

// 예상과 달리 ifdef를 걸어도 전달받은 data를 건너뛰는 건 아니다.
// 따라서 결국 mainVS의 out과 동일한 형태의 shaderDefines가 설정되어 있어야 한다.
void mainPS(
#ifdef VERTEXCOLOR
	float4 iColor : COLOR0,
#endif
#ifdef DIFFMAP
	float2 iTexCoord : TEXCOORD0,
#endif
#if !defined(BILLBOARD)
	float3 iNormal : NORMAL,
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
	float4 diffInput = DiffMapTex.Sample(DiffMapSampler, iTexCoord);

#ifdef ALPHAMASK
	if (diffInput.a < 0.5)
		discard;
#endif
	oColor = diffColor * diffInput;
#endif

	// Directional Light
	float3 DirToLight = float3(-1.0f, 1.0f, -1.0f);
	float3 DirLightColor = float3(1.0f, 1.0f, 1.0f);

	// phong diffuse
	float NDotL = dot(DirToLight, iNormal);
	float3 finalColor = DirLightColor * saturate(NDotL);

	// blinn specular
	float3 toEye = float3(0.0f, 0.0f, -10.0f) - iPosition.xyz;
	// 일단 cameraPos를 이용하면 이상해진다.
	//float3 toEye = cameraPos - iPosition.xyz;
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + DirToLight);
	float NDotH = saturate(dot(halfWay, iNormal));
	finalColor += DirLightColor * pow(NDotH, 250.0f) * 0.25;

	oColor.xyz *= finalColor;
}