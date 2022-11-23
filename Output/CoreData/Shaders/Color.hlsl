// test shader
#include "ConstantBuffers.hlsl"


// urho�� ����? �������� ���� Ÿ�Ժ� cb��
// ������ ���� ����ü�� cbubuffer�� �����Ѵ�.
// �׷��� ���� ����ü���� gpu�� �����ϴ� ����� ã�� ���ߴ�.

// position�� �⺻�̰�
// ���� ���ں��� �Ķ���ͷ� Ȯ���Ѵ�.
void mainVS(
	float4 position : POSITION,
#ifdef VERTEXCOLOR
	float4 color : COLOR,
#endif
	out float4 oPosition : SV_POSITION,
	out float4 oColor : COLOR				
)
{
	position.w = 1.0f;

	oPosition = mul(position, worldMatrix);
	oPosition = mul(oPosition, viewMatrix);
	oPosition = mul(oPosition, projectionMatrix);

#ifdef VERTEXCOLOR
	oColor = color;
#else
	oColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
#endif

}

void mainPS(
	float4 position : SV_POSITION,
	float4 color : COLOR,				// mainVS�� out�� ������ ���ƾ� �Ѵ�.
	out float4 oColor : SV_TARGET)
{
	oColor = color;
}