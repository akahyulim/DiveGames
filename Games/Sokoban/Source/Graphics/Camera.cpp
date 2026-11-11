#include "Camera.h"

namespace Sokoban
{
    void Camera::SetPerspective(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
	}

	void Camera::LookAt(DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up)
    {
        m_eye = eye;
        m_target = target;
        m_up = up;
        m_viewMatrix = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&m_eye),
            DirectX::XMLoadFloat3(&m_target),
            DirectX::XMLoadFloat3(&m_up)
        );
    }
}