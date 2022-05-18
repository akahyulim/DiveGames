
// 프레임 당 한 번 업데이트
cbuffer FrameBuffer : register(b0)
{
	matrix g_View;
	matrix g_Proj;

	float3 g_EyePos;
}

// pass 당 한 번 업데이트
// 왜 이름이 우버인지 모르겠다.
// 실제로는 MaterialBuffer 정도가 어울린다. 
cbuffer UberBuffer : register(b1)
{
	matrix g_World;

	float4 g_MatColor;

	// pdding을 꼭 맞춰줘야 한다..
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
	float3 g_LightPos		: packoffset(c0);
	float3 g_DirLightDir	: packoffset(c1);
	float3 g_LightColor		: packoffset(c2);
	float g_LightRange		: packoffset(c2.w);
	uint g_LightOptions		: packoffset(c3);
}

bool HasAlbedoTexture() { return g_MatTextures & uint(1U << 0); }
bool HasNormalTexture() { return g_MatTextures & uint(1U << 1); }

bool HasSpriteTexture() { return g_SpriteOptions & uint(1U << 0); }
bool IsSpriteFlipX() { return g_SpriteOptions & uint(1U << 1); }
bool IsSpriteFlipY() { return g_SpriteOptions & uint(1U << 2); }

// light options
bool IsDirectionalLight()	{ return g_LightOptions & uint(1U << 0); }
bool IsPointLight()			{ return g_LightOptions & uint(1U << 1); }
bool IsSpotLight()			{ return g_LightOptions & uint(1U << 2); }