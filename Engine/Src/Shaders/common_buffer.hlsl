
// 프레임 당 한 번 업데이트
cbuffer FrameBuffer : register(b0)
{
	matrix g_View;
	matrix g_Proj;
	matrix g_ViewInv;
	
	float4 g_PerspectiveValues;

	float3 g_EyePos;
}

// pass 당 한 번 업데이트
// 왜 이름이 우버인지 모르겠다.
// 실제로는 MaterialBuffer 정도가 어울린다. 
cbuffer UberBuffer : register(b1)
{
	matrix g_World;

	float4 g_MtrlColor;

	float2 g_MtrlTiling;
	float2 g_MtrlOffset;

	// pdding을 꼭 맞춰줘야 한다..
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

// 책에서는 shader에 따라 register를 설정해 놓았다.
// 예를 들면, directional, point, spot 각각의 shader에 register(b2)를 만들어 사용한다.
cbuffer LightBuffer : register(b3)
{
	// 이렇게 하는 게 깔끔하다.
	// 그리고 사용할 때도 명시적으로 x, y, z, w를 표시해주는 편이 낫다.
	// 일단 4byre 단위인 듯 한데, matrix는 어떻게 될까?
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