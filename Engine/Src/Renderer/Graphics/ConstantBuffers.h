#pragma once
#include <DirectXMath.h>

// 이름을 바꿔야 할 것 같다.
namespace Dive
{
	struct BufferFrame
	{
		DirectX::XMMATRIX GetViewMatrix() const { return DirectX::XMLoadFloat4x4(&view); }
		void SetViewMatrix(const DirectX::FXMMATRIX& mat) { DirectX::XMStoreFloat4x4(&view, mat); }

		DirectX::XMMATRIX GetPerspectiveProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&projPerspective); }
		void SetPerspectiveProjectionMatrix(const DirectX::FXMMATRIX& mat) { DirectX::XMStoreFloat4x4(&projPerspective, mat); }

		DirectX::XMMATRIX GetOrthoProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&projOrtho); }
		void SetOrthoProjectionMatrix(const DirectX::FXMMATRIX& mat) { DirectX::XMStoreFloat4x4(&projOrtho, mat); }

		DirectX::XMMATRIX GetViewProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&viewProj); }
		void SetViewProjectionMatrix(const DirectX::FXMMATRIX& mat) { DirectX::XMStoreFloat4x4(&viewProj, mat); }

		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projPerspective;
		DirectX::XMFLOAT4X4 projOrtho;
		DirectX::XMFLOAT4X4 viewProj;

		// 일단 비교 함수는 뺀다.
	};

	struct BufferObject
	{
		DirectX::XMMATRIX GetWorldMatrix() const { return DirectX::XMLoadFloat4x4(&world); }
		void SetWorldMatrix(const DirectX::FXMMATRIX& mat) { DirectX::XMStoreFloat4x4(&world, mat); }

		DirectX::XMMATRIX GetWorldViewProjectionMatrix() const { return DirectX::XMLoadFloat4x4(&wvp); }
		void SetWorldViewProjectionMatrix(const DirectX::FXMMATRIX& mat) { DirectX::XMStoreFloat4x4(&wvp, mat); }

		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 wvp;
	};
}