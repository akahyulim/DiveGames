
// ������ �� �� �� ������Ʈ
cbuffer FrameBuffer : register(b0)
{
	matrix world;	// �̰� ObjectBuffer�� �Űܾ� �Ѵ�.
	matrix view;
	matrix proj;
}

// pass �� �� �� ������Ʈ
// �� �̸��� ������� �𸣰ڴ�.
cbuffer UberBuffer : register(b1)
{
	float4 materialColor;
}

// ������Ʈ �� ������Ʈ
cbuffer ObjectBuffer : register(b2)
{
	// object world
	// wvp
	// wvp_old
}

// light ���� ������Ʈ
cbuffer LightBuffer : register(b3)
{

}