#pragma once
#include "CommonInclude.h"

namespace dive
{
	struct Vertex_Pos
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};

	struct Vertex_Color
	{
		DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	};

	struct Vertex_Normal
	{
		DirectX::XMFLOAT3 nor = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};
}