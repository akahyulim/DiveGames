#pragma once
#include "Component.h"

namespace Dive
{
	class Camera : public Component
	{
		COMPONENT_CLASS_TYPE(Camera)

	public:
		Camera(GameObject* pGameObject);
		// 아직 모르겠지만 아래 생성자는 없애는 편이...
		Camera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat);
		virtual ~Camera() = default;

		const DirectX::XMFLOAT4X4& GetProjection() const { return m_Projection; }

	protected:
		DirectX::XMFLOAT4X4 m_Projection;
	};
}