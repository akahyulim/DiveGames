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
#ifdef LIGHT
	//out float3 oToEye : TEXCOORD1,
	out float3 oWorldPos : TEXCOORD1,
#endif
	out float4 oPosition : SV_POSITION
)
{
	iPosition.w = 1.0f;

	float4 worldPos = mul(iPosition, cModel);
	oPosition = mul(worldPos, viewMatrix);
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

#ifdef LIGHT
	//oToEye = cameraPos - worldPos.xyz;
	//oToEye = normalize(oToEye);
	oWorldPos = worldPos.xyz;
#endif
}

float4 loadTex(float2 texCoord)
{
	uint width, height;
	DiffMapTex.GetDimensions(width, height);
	float3 texCoord3 = float3(texCoord.x * width, texCoord.y * height, 0);
	return DiffMapTex.Load(texCoord3);
}

// ����� �޸� ifdef�� �ɾ ���޹��� data�� �ǳʶٴ� �� �ƴϴ�.
// ���� �ᱹ mainVS�� out�� ������ ������ shaderDefines�� �����Ǿ� �־�� �Ѵ�.
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
#ifdef LIGHT
	//float3 iToEye : TEXCOORD1,
	float3 iWorldPos : TEXCOORD1,
#endif
	float4 iPosition : SV_POSITION,
	out float4 oColor : SV_TARGET)
{
	// urho�� cMatDiffColor�̶�°� ���� �����Ѵ�.
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

#ifdef LIGHT																		  
	float3 dirToLight = -lightDir;
	float3 DirLightColor = lightColor.xyz;

	// phong diffuse
	float NDotL = dot(dirToLight, iNormal);
	float3 finalColor = DirLightColor * saturate(NDotL);

	// blinn specular
	float3 toEye = cameraPos - iWorldPos;
	toEye = normalize(toEye);
	float3 halfWay = normalize(toEye + dirToLight);
	float NDotH = saturate(dot(halfWay, iNormal));
	//finalColor += DirLightColor * pow(NDotH, 250.0f) * 0.25f;		
	finalColor += DirLightColor * pow(NDotH, 32.0f);

	oColor.xyz *= finalColor;
#endif
}