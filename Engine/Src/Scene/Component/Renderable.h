#pragma once
#include "Component.h"

namespace Dive
{
	class Material;
	class GameObject;
	
	class Renderable : public Component
	{
	public:
		Renderable(GameObject* pGameObject) : Component(pGameObject) {}
		virtual ~Renderable() = default;

		// Behaviour의 Enalbed랑은 다르다. 이건 보이느냐 마느냐이다.
		bool IsEnabled() const { return m_bEnabled; }
		void SetEnable(bool enable) { m_bEnabled = enable; }

		bool IsVisible() const { return m_bVisible; }

		Material* GetMaterial() { return m_pMaterial; }
		void SetMaterial(Material* pMaterial);
		bool HasMaterial() const { return m_pMaterial != nullptr; }

	protected:
	protected:
		// 버퍼
		// count
		// stride

		// 유니티엔 요소가 좀 많다.
		bool m_bEnabled = true;
		bool m_bVisible = true;	// 이건 컬링 유무를 리턴하는데 사용하는 것 같다.

		Material* m_pMaterial = nullptr;
	};
}