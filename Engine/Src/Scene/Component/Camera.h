#pragma once
#include "Component.h"

namespace Dive
{
	class Camera : public Component
	{
	public:
		Camera(GameObject* pGameObject);
		Camera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat);
		virtual ~Camera() = default;

		const DirectX::XMFLOAT4X4& GetProjection() const { return m_Projection; }

	protected:
		DirectX::XMFLOAT4X4 m_Projection;
	};
}