#pragma once
#include "GraphicsInclude.h"

// 이름을 바꿔야 할 것 같다.
namespace dive
{
	// 일단 이정도만
	// 추후 더 추가
	struct BufferFrame
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projPerspective;
		DirectX::XMFLOAT4X4 projOrtho;
		DirectX::XMFLOAT4X4 viewProj;

		// 일단 비교 함수는 뺀다.
	};

	struct BufferObject
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 wvp;
	};
}