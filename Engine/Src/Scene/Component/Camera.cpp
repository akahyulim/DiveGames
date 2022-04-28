#include "divepch.h"
#include "Camera.h"

namespace Dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(pGameObject)
	{
		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	}

	Camera::Camera(GameObject* pGameObject, unsigned int long long id)
		: Component(pGameObject, id)
	{
		DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	}

	Camera::Camera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat)
		: Component(pGameObject), m_Projection(mat)
	{
	}
	
	Camera::Camera(GameObject* pGameObject, unsigned int long long id, const DirectX::XMFLOAT4X4& mat)
		:Component(pGameObject, id), m_Projection(mat)
	{
	}
}