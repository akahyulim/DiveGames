
// ������ �� �� �� ������Ʈ
cbuffer FrameBuffer : register(b0)
{
	matrix g_view;
	matrix g_proj;
};

// pass �� �� �� ������Ʈ
// �� �̸��� ������� �𸣰ڴ�.
// �����δ� MaterialBuffer ������ ��︰��. 
cbuffer UberBuffer : register(b1)
{
	matrix g_world;

	float4 g_mat_color;

	// pdding�� �� ������� �Ѵ�..
	uint g_mat_textures;
	uint3 g_padding;
};

bool HasAlbedoTexture() { return g_mat_textures & uint(1U << 0); }
bool HasNormalTexture() { return g_mat_textures & uint(1U << 1); }