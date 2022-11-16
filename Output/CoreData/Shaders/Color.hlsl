// test shader

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

// position은 기본이고
// 이후 인자부턴 파라미터로 확인한다.
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
	float4 color : COLOR,				// mainVS의 out과 순서가 같아야 한다.
	out float4 oColor : SV_TARGET)
{
	oColor = color;
}