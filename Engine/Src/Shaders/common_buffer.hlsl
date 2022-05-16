
// 프레임 당 한 번 업데이트
cbuffer FrameBuffer : register(b0)
{
	matrix g_view;
	matrix g_proj;
}

// pass 당 한 번 업데이트
// 왜 이름이 우버인지 모르겠다.
// 실제로는 MaterialBuffer 정도가 어울린다. 
cbuffer UberBuffer : register(b1)
{
	matrix g_world;

	float4 g_mat_color;

	// pdding을 꼭 맞춰줘야 한다..
	uint g_mat_textures;
	uint3 g_padding;
}

cbuffer SpriteBuffer : register(b2)
{
	matrix g_sprite_world;
	
	float4 g_sprite_color;

	uint g_sprite_options;
	uint3 g_sprite_padding;
}

cbuffer LightBuffer : register(b3)
{
	float3 g_dir_light_dir;
	float3 g_light_color;
}

bool HasAlbedoTexture() { return g_mat_textures & uint(1U << 0); }
bool HasNormalTexture() { return g_mat_textures & uint(1U << 1); }

bool HasSpriteTexture() { return g_sprite_options & uint(1U << 0); }
bool IsSpriteFlipX() { return g_sprite_options & uint(1U << 1); }
bool IsSpriteFlipY() { return g_sprite_options & uint(1U << 2); }