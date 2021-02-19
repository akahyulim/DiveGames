#pragma once
#include "DivePch.h"

namespace Dive
{
	// 각종 Constant Buffer 구조체 선언

	struct CB_OBJECT
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 worldViewProjection;
	};
}