#pragma once
#include "Component.h"
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace Dive
{
	class Transform : public Component
	{
	public:
		Transform();
		~Transform();

		void Update(float deltaTime) override;

		// transform

		// hierarchy
		Transform* GetRoot();
		bool HasParent() const { return (m_pParent == nullptr); }
		Transform* GetParent() const { return m_pParent; }
		void SetParent(Transform* pParent);
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_children.size()); }
		Transform* Find(const std::string& name) const;
		Transform* GetChild(unsigned int index) const;
		bool IsChildOf(const Transform* pParent) const;
		void DetachChildren();

	private:

	private:
		// transform

		// heirarchy
		Transform* m_pParent = nullptr;
		std::vector<Transform*> m_children;
	};
}