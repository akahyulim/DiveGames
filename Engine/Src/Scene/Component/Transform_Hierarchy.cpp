#include "divepch.h"
#include "Transform.h"
#include "../SceneManager.h"
#include "../Scene.h"
#include "../GameObject.h"

namespace Dive
{
	//==================================================//
	// ���� nullptr�� ��ȯ���� �ʽ��ϴ�.				//
	// �θ� �������� ���� ��� �ڱ��ڽ��� ��ȯ�մϴ�.	//
	//==================================================//
	Transform* Transform::GetRoot()
	{
		if (!m_pParent)
			return this;
		else
			return m_pParent->GetRoot();
	}

	// ��� ���� �ذ�
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
					CORE_TRACE("{0:s}�� {1:s}(��)�� �������׽��ϴ�.", GetParent()->GetGameObject()->GetName(), GetGameObject()->GetName());

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
			CORE_TRACE("{0:s}�� {1:s}�� ���ο� �θ� �Ǿ����ϴ�.", pParent->GetGameObject()->GetName(), GetGameObject()->GetName());
		}
		m_pParent = pParent;
	

		// ����... ���� ���踦 �����غ��� �Ѵ�.
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

	// parent�� �ڽ��̶� ture�� �����Ѵ�.
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

		// �� ���� ������ �����ؾ� �ϳ�...?
		//updateTransform();
		m_bChanged = true;
	}
}