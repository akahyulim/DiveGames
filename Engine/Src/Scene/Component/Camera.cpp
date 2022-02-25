#include "divepch.h"
#include "Camera.h"

namespace Dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
	{
		m_Type = eComponentType::Camera;

		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	}

	Camera::Camera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat)
		: Component(pGameObject), m_Projection(mat)
	{
		m_Type = eComponentType::Camera;
	}
}