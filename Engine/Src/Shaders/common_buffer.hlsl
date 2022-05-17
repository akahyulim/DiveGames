
// ������ �� �� �� ������Ʈ
cbuffer FrameBuffer : register(b0)
{
	matrix g_View;
	matrix g_Proj;

	float3 g_EyePos;
}

// pass �� �� �� ������Ʈ
// �� �̸��� ������� �𸣰ڴ�.
// �����δ� MaterialBuffer ������ ��︰��. 
cbuffer UberBuffer : register(b1)
{
	matrix g_World;

	float4 g_MatColor;

	// pdding�� �� ������� �Ѵ�..
	uint g_MatTextures;
	uint3 g_Padding;
}

cbuffer SpriteBuffer : register(b2)
{
	matrix g_SpriteWorld;
	
	float4 g_SpriteColor;

	uint g_SpriteOptions;
	uint3 g_SpritePadding;
}

cbuffer LightBuffer : register(b3)
{
	float3 g_DirLightDir;
	float3 g_LightColor;
}

bool HasAlbedoTexture() { return g_MatTextures & uint(1U << 0); }
bool HasNormalTexture() { return g_MatTextures & uint(1U << 1); }

bool HasSpriteTexture() { return g_SpriteOptions & uint(1U << 0); }
bool IsSpriteFlipX() { return g_SpriteOptions & uint(1U << 1); }
bool IsSpriteFlipY() { return g_SpriteOptions & uint(1U << 2); }