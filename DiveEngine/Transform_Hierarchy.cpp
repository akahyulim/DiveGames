#include "Transform.h"
#include "GameObject.h"

namespace Dive
{
	Transform* Transform::GetRoot()
	{
		if (m_pParent == nullptr)
			return this;
		else
			return m_pParent->GetRoot();
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent == nullptr || this->GetID() == pParent->GetID())
			return;

		if (m_pParent != nullptr)
		{
			// 항상 헷갈린다...
			auto it = m_pParent->m_children.begin();
			for (it; it != m_pParent->m_children.end();)
			{
				if ((*it)->GetID() == this->GetID())
				{
					it = m_pParent->m_children.erase(it++);
				}
				else
					it++;
			}
		}

		pParent->m_children.emplace_back(this);
		m_pParent = pParent;
	}

	Transform* Transform::Find(const std::string& name) const
	{
		if (!m_children.empty())
		{
			for (auto& child : m_children)
			{
				if (child->GetOwner() && child->GetOwner()->GetName() == name)
				{
					return child;
				}
			}
		}

		return nullptr;
	}

	Transform* Transform::GetChild(unsigned int index) const
	{
		if (m_children.empty() || m_children.size() - 1 < index)
			return nullptr;

		return m_children[index];
	}

	// 자식의 자식, 자기 자신도 true
	bool Transform::IsChildOf(const Transform* pParent) const
	{
		assert(pParent != nullptr);

		if (this->GetID() == pParent->GetID())
			return true;

		if (m_pParent == nullptr)
			return false;

		if (m_pParent->GetID() == pParent->GetID())
			return true;

		return m_pParent->IsChildOf(pParent);
	}

	void Transform::DetachChildren()
	{
		for (auto& child : m_children)
		{
			child->m_pParent = nullptr;
		}

		m_children.clear();
		m_children.shrink_to_fit();
	}
}