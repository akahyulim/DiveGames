
// ������ �� �� �� ������Ʈ
cbuffer FrameBuffer : register(b0)
{
	matrix g_View;
	matrix g_Proj;
	matrix g_ViewInv;
	
	float4 g_PerspectiveValues;

	float3 g_EyePos;
}

// pass �� �� �� ������Ʈ
// �� �̸��� ������� �𸣰ڴ�.
// �����δ� MaterialBuffer ������ ��︰��. 
cbuffer UberBuffer : register(b1)
{
	matrix g_World;

	float4 g_MtrlColor;

	float2 g_MtrlTiling;
	float2 g_MtrlOffset;

	// pdding�� �� ������� �Ѵ�..
	uint g_MtrlTextures;
	uint3 g_Padding;
}

cbuffer SpriteBuffer : register(b2)
{
	matrix g_SpriteWorld;
	
	float4 g_SpriteColor;

	uint g_SpriteOptions;
	uint3 g_SpritePadding;
}

// å������ shader�� ���� register�� ������ ���Ҵ�.
// ���� ���, directional, point, spot ������ shader�� register(b2)�� ����� ����Ѵ�.
cbuffer LightBuffer : register(b3)
{
	// �̷��� �ϴ� �� ����ϴ�.
	// �׸��� ����� ���� ��������� x, y, z, w�� ǥ�����ִ� ���� ����.
	// �ϴ� 4byre ������ �� �ѵ�, matrix�� ��� �ɱ�?
	float3 g_LightPos		: packoffset(c0);
	float g_LightRangeRcp	: packoffset(c0.w);
	
	float3 g_LightDir		: packoffset(c1);
	float g_SpotAngle		: packoffset(c1.w);

	float3 g_LightColor		: packoffset(c2);
	uint g_LightOptions		: packoffset(c2.w);
}

bool HasAlbedoTexture() { return g_MtrlTextures & uint(1U << 0); }
bool HasNormalTexture() { return g_MtrlTextures & uint(1U << 1); }

bool HasSpriteTexture() { return g_SpriteOptions & uint(1U << 0); }
bool IsSpriteFlipX() { return g_SpriteOptions & uint(1U << 1); }
bool IsSpriteFlipY() { return g_SpriteOptions & uint(1U << 2); }

// light options
bool IsDirectionalLight()	{ return g_LightOptions & uint(1U << 0); }
bool IsPointLight()			{ return g_LightOptions & uint(1U << 1); }
bool IsSpotLight()			{ return g_LightOptions & uint(1U << 2); }