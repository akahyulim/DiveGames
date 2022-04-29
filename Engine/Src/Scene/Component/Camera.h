#pragma once
#include "Component.h"

namespace Dive
{
	class Camera : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Camera)

		Camera(GameObject* pGameObject, unsigned int long long id = 0);
		// 아직 모르겠지만 아래 두 생성자는 없애는 편이...
		Camera(GameObject* pGameObject, const DirectX::XMFLOAT4X4& mat);
		Camera(GameObject* pGameObject, unsigned int long long id, const DirectX::XMFLOAT4X4& mat);
		virtual ~Camera() = default;

		const DirectX::XMFLOAT4X4& GetProjection() const { return m_Projection; }

	protected:
		DirectX::XMFLOAT4X4 m_Projection;
	};
}