#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;

	class Transform : public Component
	{
	public:
		Transform(GameObject* pGameObject);
		~Transform() = default;

		bool HasParent() const;
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		Transform* GetRoot();
		bool HasChild() const { return !m_Children.empty(); }
		uint32_t GetChildCount() const { return static_cast<uint32_t>(m_Children.size()); }
		Transform* GetChild(uint32_t index) const;
		bool IsChildOf(Transform* pParent) const;
		void DetachChildren();
		std::vector<Transform*> GetChildren() const { return m_Children; }

	private:
	private:
		Transform* m_pParent;
		std::vector<Transform*> m_Children;
	};
}