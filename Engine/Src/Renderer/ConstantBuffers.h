#pragma once
#include <DirectXMath.h>

namespace Dive
{
	// 크기 설정을 1바이트 기준을 하기 위한 조치 같다.
	// 책에서 이렇게 했다.
#pragma pack(push, 1)

	struct FrameBuffer
	{
		DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX proj = DirectX::XMMatrixIdentity();

		DirectX::XMFLOAT3 eyePos = {0.0f, 0.0f, 0.0f};
		float pad;
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
		DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
		float pad;

		DirectX::XMFLOAT3 dir = { 0.0f, 0.0f, 0.0f };
		float pad1;
		
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		float range = 0.0f;

		unsigned int options = 0;
		float pad3[3] = { 0 };
	};

#pragma pack(pop)
}