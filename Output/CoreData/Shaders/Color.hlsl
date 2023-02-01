// test shader
#include "ConstantBuffers.hlsl"


// urho�� ����? �������� ���� Ÿ�Ժ� cb��
// ������ ���� ����ü�� cbuffer�� �����Ѵ�.
// �׷��� ���� ����ü���� gpu�� �����ϴ� ����� ã�� ���ߴ�.

// �Է��� �⺻�� Geometry Ÿ������ ������ ������ ���޹޾� �����ϰ�
// =>(urho�� ��� Renderer�� ���� �迭�� �̹� ���� �Ǿ� �ִ�.)
// �׿�(DIFFMAP ��)���� Tech�� Pass�� ����� Define���� ���޹޴´�. 
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
	out float4 oColor : SV_TARGET)		// urho�� OUTCOLOR0�̶�� �ø�ƽ�� ����ϴµ� ���� �ȵȴ�. SV_TARGET�� �� �ϴ�.
{
#ifdef VERTEXCOLOR
	oColor = iColor;
#else
	oColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
#endif

}