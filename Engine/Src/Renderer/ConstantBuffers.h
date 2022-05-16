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

	struct SpriteBuffer
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		DirectX::XMFLOAT4 materialColor = { 0.0f, 0.0f, 0.0f, 0.0f };

		unsigned int options = 0;
		unsigned int padding[3] = { 0 };
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT3 dir = { 0.0f, 0.0f, 0.0f };
		float pad;
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float pad2;
	};
}