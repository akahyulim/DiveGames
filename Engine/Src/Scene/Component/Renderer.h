#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	// 이름의 통일성이 깨졌다.
	// 어느쪽을 바꿔야 할까...
	class RendererComponent : public Component
	{
	public:
		COMPONENT_CLASS_TYPE(Unknown)

		RendererComponent(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~RendererComponent() = default;

		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

		bool IsVisible() const { return m_bVisible; }

	protected:
	protected:
		// 유니티엔 요소가 좀 많다.
		bool m_bEnabled = true;
		bool m_bVisible = true;	// 이건 컬링 유무를 리턴하는데 사용하는 것 같다.
	};
}