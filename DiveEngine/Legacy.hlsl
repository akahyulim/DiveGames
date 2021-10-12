cbuffer BufferFrameGPU
{
	// float4x4와 matrix는 차이가 없나?
	matrix world;
	matrix wvp;
};

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

PixelInput mainVS(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;
	output.position = mul(input.position, wvp);

	output.uv = input.uv;
	output.normal = mul(input.normal, (float3x3)world).xyz;
	output.tangent = mul(input.tangent, (float3x3)world).xyz;

	return output;
}

// diffuse texture and sampler
Texture2D DiffuseTexture	: register(t0);
SamplerState LinearSampler	: register(s0);

// 에디터의 경우 RenderTarget이 달라야하지 않나?
// 현재 그냥 보인다.
float4 mainPS(PixelInput input) : SV_TARGET
{
	// DiffuseTexture가 없을때 어떻게 되는지 보고 싶다.
	float3 DiffuseColor = DiffuseTexture.Sample(LinearSampler, input.uv);

	// 샘플링한 색을 제곱한 이유는 아직 모른다.
	DiffuseColor *= DiffuseColor;

	// 아에 그려지지 않는다.
	// 그렇다면 책에서도 DiffuseTexture가 어떻게든 초기화 되었다는 이야기이다.
	//return float4(DiffuseColor, 1.0f);
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}