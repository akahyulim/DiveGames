
// 프레임 당 한 번 업데이트
cbuffer FrameBuffer : register(b0)
{
	matrix world;	// 이건 ObjectBuffer로 옮겨야 한다.
	matrix view;
	matrix proj;
}

// pass 당 한 번 업데이트
// 왜 이름이 우버인지 모르겠다.
cbuffer UberBuffer : register(b1)
{
	float4 materialColor;
}

// 오브젝트 당 업데이트
cbuffer ObjectBuffer : register(b2)
{
	// object world
	// wvp
	// wvp_old
}

// light 마다 업데이트
cbuffer LightBuffer : register(b3)
{

}