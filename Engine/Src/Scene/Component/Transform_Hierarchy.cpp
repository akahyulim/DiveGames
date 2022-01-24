#include "divepch.h"
#include "Transform.h"
#include "../SceneManager.h"
#include "../Scene.h"
#include "../GameObject.h"

namespace Dive
{
	//==================================================//
	// 절대 nullptr을 반환하지 않습니다.				//
	// 부모가 존재하지 않을 경우 자기자신을 반환합니다.	//
	//==================================================//
	Transform* Transform::GetRoot()
	{
		if (!m_pParent)
			return this;
		else
			return m_pParent->GetRoot();
	}

	// 모든 문제 해결
	void Transform::SetParent(Transform* pParent)
	{
		if (pParent)
		{
			if (pParent->GetInstanceID() == GetInstanceID())
				return;
		}

		if (HasParent())
		{
			if (pParent)
			{
				if (pParent->GetInstanceID() == GetParent()->GetInstanceID())
					return;
			}

			auto& sibling = GetParent()->m_Children;
			for (auto it = sibling.begin(); it != sibling.end();)
			{
				if ((*it)->GetInstanceID() == GetInstanceID())
				{
					CORE_TRACE("{0:s}가 {1:s}(을)를 독립시켰습니다.", GetParent()->GetGameObject()->GetName(), GetGameObject()->GetName());

					sibling.erase(it);
					break;
				}
				else
					++it;
			}
		}

		if (pParent)
		{
			pParent->m_Children.emplace_back(this);
			CORE_TRACE("{0:s}가 {1:s}의 새로운 부모가 되었습니다.", pParent->GetGameObject()->GetName(), GetGameObject()->GetName());
		}
		m_pParent = pParent;
	

		// 흐음... 둘의 관계를 생각해봐야 한다.
		//updateTransform();
		m_bChanged = true;
	}

	Transform* Transform::GetChildByName(const std::string& name) const
	{
		if (!m_Children.empty())
		{
			for (auto child : m_Children)
			{
				assert(child->GetGameObject() && "Do not find GameObject");

				if (child->GetGameObject()->GetName() == name)
				{
					return child;
				}
			}
		}

		return nullptr;
	}

	// parent가 자신이라도 ture를 리턴한다.
	bool Transform::IsChildOf(const Transform* pParent) const
	{
		if (!pParent)
			return false;
		
		if (pParent->GetInstanceID() == GetInstanceID())
			return true;

		if (!HasParent())
			return false;

		for (auto child : pParent->GetChildren())
		{
			if (child->GetInstanceID() == GetInstanceID())
				return true;
		}

		return m_pParent->IsChildOf(pParent);
	}

	void Transform::DetachChildren()
	{
		if (m_Children.empty())
			return;

		for (auto pChild : m_Children)
		{
			pChild->m_pParent = nullptr;
		}

		m_Children.clear();
		m_Children.shrink_to_fit();

		// 이 둘을 별개로 관리해야 하나...?
		//updateTransform();
		m_bChanged = true;
	}
}