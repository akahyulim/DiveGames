cbuffer MatrixBuffer
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
	float4x4 projOrtho;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

PixelInput mainVS(VertexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;
	
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.uv = input.uv;

	return output;
}

Texture2D shaderTex;
SamplerState samplerType;

// ���÷��� ������� �ʰ� �ؼ� �б�
float4 texLoad(Texture2D tex, float2 texCoord)
{
	// texture�� ũ�� ȹ��
	uint width, height;
	tex.GetDimensions(width, height);
	
	// texcoord�� �Բ� �׷��� ��ǥ ��� + mip level
	float3 texCoord3 = float3(texCoord.x * width,
		texCoord.y * height,
		0);

	// �ش� ��ǥ�� �ؼ��� �ε�
	return tex.Load(texCoord3);
}

float4 mainPS(PixelInput input) : SV_TARGET
{
//	float4 color;
//	color = shaderTex.Sample(samplerType, input.uv);

	//return texLoad(shaderTex, input.uv);

	// ��Ʈ �׽�Ʈ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ��Ʈ�� ��� ����

	 float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	 color.r = shaderTex.Sample(samplerType, input.uv);
	 color.g = color.r;
	 color.b = color.r;
	 color.a = color.r;

	 // Color it
	 // ���� ���ߴ�.
	 color *= float4(0.3f, 0.3f, 0.3f, 1.0f);
	 
	 return color;
}