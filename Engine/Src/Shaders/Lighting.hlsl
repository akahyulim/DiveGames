#include "common.hlsl"

// vertex shade
// input을 뭘로 해야 하지?
// 이름처럼 light로 해야하나..?
// 하지만 대부분 cb로 전달받는 걸?

Texture2D<float> DepthTexture		: register(t0);
Texture2D<float4> AlbedoTexture		: register(t1);
Texture2D<float4> NormalTexture		: register(t2);
Texture2D<float4> MaterialTexture	: register(t3);

struct Pixel_Output
{
	float4 FrameOutput : SV_TARGET0;
};

// pixel shade
// 일단 depth를 선형으로 출력한다.