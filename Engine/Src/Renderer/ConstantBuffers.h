#pragma once
#include <DirectXMath.h>

namespace Dive
{
	// 크기 설정을 1바이트 기준을 하기 위한 조치 같다.
	// 책에서 이렇게 했다.
#pragma pack(push, 1)

	struct FrameBuffer
	{
		DirectX::XMMATRIX view		= DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX proj		= DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX viewInv	= DirectX::XMMatrixIdentity();

		DirectX::XMFLOAT4 perspectiveValues = { 0.0f, 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT3 eyePos = {0.0f, 0.0f, 0.0f};
		float pad;
	};

	struct UberBuffer
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

		DirectX::XMFLOAT4 mtrlColor = { 0.0f, 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT2 mtrlTiling = { 1.0f, 1.0f };
		DirectX::XMFLOAT2 mtrlOffset = { 0.0f, 0.0f };

		unsigned int mtrlTextures = 0;
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
		float rangeRcp = 0.0f;

		DirectX::XMFLOAT3 dir = { 0.0f, 0.0f, 0.0f };
		float spotAngle = 0.0f;
		
		DirectX::XMFLOAT3 color = { 0.0f, 0.0f, 0.0f };
		unsigned int options = 0;
	};

#pragma pack(pop)
}