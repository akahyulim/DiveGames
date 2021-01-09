
// cbuffer
cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

// input
struct VertexInput
{
	float4 position : POSITION0;
	float2 uv		: TEXCOORD0;
	float3 normal	: NORMAL0;
	float3 tangent	: TANGENT0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal	: TEXCOORD1;
	float3 tangent	: TEXCOORD2;
};

// vs main
PixelInput mainVS(VertexInput input)
{
	PixelInput output = (PixelInput)0;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	return output;
}

// ps main
float4 mainPS(PixelInput input) : SV_TARGET
{
	return float4(0.5f, 0.5f, 0.0f, 1.0f);
}