#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

namespace dive
{
	//==================================================//
	// 절대 nullptr을 반환하지 않습니다.				//
	// 부모가 존재하지 않을 경우 자기자신을 반환합니다.	//
	//==================================================//
	Transform* Transform::GetRoot()
	{
		if (!m_Parent)
			return this;
		else
			return m_Parent->GetRoot();
	}

	// 모든 문제 해결
	void Transform::SetParent(Transform* parent)
	{
		if (parent)
		{
			if (parent->GetInstanceID() == GetInstanceID())
				return;
		}

		if (HasParent())
		{
			if (parent)
			{
				if (parent->GetInstanceID() == GetParent()->GetInstanceID())
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

		if (parent)
		{
			parent->m_Children.emplace_back(this);
			CORE_TRACE("{0:s}가 {1:s}의 새로운 부모가 되었습니다.", parent->GetGameObject()->GetName(), GetGameObject()->GetName());
		}
		m_Parent = parent;
	

		// 흐음... 둘의 관계를 생각해봐야 한다.
		UpdateTransform();
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
	bool Transform::IsChildOf(const Transform* parent) const
	{
		if (!parent)
			return false;
		
		if (parent->GetInstanceID() == GetInstanceID())
			return true;

		if (!HasParent())
			return false;

		for (auto child : parent->GetChildren())
		{
			if (child->GetInstanceID() == GetInstanceID())
				return true;
		}

		return m_Parent->IsChildOf(parent);
	}

	void Transform::DetachChildren()
	{
		if (m_Children.empty())
			return;

		for (auto pChild : m_Children)
		{
			pChild->m_Parent = nullptr;
		}

		m_Children.clear();
		m_Children.shrink_to_fit();

		// 이 둘을 별개로 관리해야 하나...?
		UpdateTransform();
		m_bChanged = true;
	}
}