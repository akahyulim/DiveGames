#pragma once
#include "Component.h"

namespace Dive
{
	class Camera : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Camera)

		Camera(GameObject* pGameObject);
		Camera(GameObject* pGameObject, unsigned int long long id);
		Camera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat);
		Camera(GameObject* pGameObject, unsigned int long long id, const DirectX::XMFLOAT4X4& mat);
		virtual ~Camera() = default;

		const DirectX::XMFLOAT4X4& GetProjection() const { return m_Projection; }

	protected:
		DirectX::XMFLOAT4X4 m_Projection;
	};
}