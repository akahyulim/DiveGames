#pragma once
#include <DirectXMath.h>

namespace Sokoban
{
	class Camera
	{
	public:
		Camera() = default;
		~Camera() = default;

        void SetPerspective(float fovAngleY, float aspectRatio, float nearZ, float farZ);

		void LookAt(DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);

        DirectX::XMMATRIX GetViewMatrix() const { return m_viewMatrix; }
        DirectX::XMMATRIX GetProjectionMatrix() const { return m_projectionMatrix; }

	private:
	private:
		DirectX::XMFLOAT3 m_eye{ 0.0f, 15.0f, -10.0f };
		DirectX::XMFLOAT3 m_target{ 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_up{ 0.0f, 1.0f, 0.0f };

		DirectX::XMMATRIX m_viewMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX m_projectionMatrix = DirectX::XMMatrixIdentity();
	};
}