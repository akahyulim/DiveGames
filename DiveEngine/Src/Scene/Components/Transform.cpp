#include "DivePch.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "IO/Log.h"

namespace Dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject),
		m_pParent(nullptr)
	{

	}

	bool Transform::HasParent() const
	{
		return m_pParent;
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent)
		{
			if (pParent->GetID() == this->GetID())
				return;

			if (m_pParent)
			{
				if (m_pParent->GetID() == pParent->GetID())
					return;

				// 자식을 부모로 삼을 수 없다.
				if (pParent->IsChildOf(this))
					return;
				else
				{
					auto it = m_pParent->m_Children.begin();
					for (it; it != m_pParent->m_Children.end(); ++it)
					{
						if ((*it)->GetID() == this->GetID())
						{
							m_pParent->m_Children.erase(it);
							break;
						}
					}

					m_pParent = pParent;
					pParent->m_Children.emplace_back(this);
				}
			}
			else
			{
				m_pParent = pParent;
				pParent->m_Children.emplace_back(this);
			}
		}
		else
		{
			if (m_pParent)
			{
				auto it = m_pParent->m_Children.begin();
				for (it; it != m_pParent->m_Children.end(); ++it)
				{
					if ((*it)->GetID() == this->GetID())
					{
						m_pParent->m_Children.erase(it);
						break;
					}
				}
				m_pParent = nullptr;
			}
		}
	}

	Transform* Transform::GetRoot()
	{
		if (!m_pParent)
			return this;

		return m_pParent->GetRoot();
	}

	Transform* Transform::GetChild(uint32_t index) const
	{
		if (m_Children.empty() || static_cast<uint32_t>(m_Children.size()) < index)
			return nullptr;

		return m_Children[index];
	}

	bool Transform::IsChildOf(Transform* pParent) const
	{
		if (!pParent)
			return false;

		if (this->GetID() == pParent->GetID())
			return true;

		if (!m_pParent)
			return false;
		else
			return m_pParent->IsChildOf(pParent);
	}

	void Transform::DetachChildren()
	{
		for (auto* pChild : m_Children)
		{
			pChild->SetParent(nullptr);
		}
		m_Children.clear();
		m_Children.shrink_to_fit();
	}
}