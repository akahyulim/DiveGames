#pragma once
#include <DirectXMath.h>

namespace Dive
{
	struct FrameBuffer
	{
		DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX proj = DirectX::XMMatrixIdentity();
	};

	struct UberBuffer
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		DirectX::XMFLOAT4 materialColor = { 0.0f, 0.0f, 0.0f, 0.0f };

		unsigned int materialTextures = 0;
		unsigned int padding[3] = { 0 };
	};
}