#pragma once
#include "DivePch.h"

namespace Dive
{
	// ���� Constant Buffer ����ü ����

	struct CB_OBJECT
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 worldViewProjection;
	};
}