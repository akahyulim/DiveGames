#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

namespace dive
{
	//==================================================//
	// ���� nullptr�� ��ȯ���� �ʽ��ϴ�.				//
	// �θ� �������� ���� ��� �ڱ��ڽ��� ��ȯ�մϴ�.	//
	//==================================================//
	Transform* Transform::GetRoot()
	{
		if (!m_Parent)
			return this;
		else
			return m_Parent->GetRoot();
	}

	// ��� ���� �ذ�
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
					CORE_TRACE("{0:s}�� {1:s}(��)�� �������׽��ϴ�.", GetParent()->GetGameObject()->GetName(), GetGameObject()->GetName());

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
			CORE_TRACE("{0:s}�� {1:s}�� ���ο� �θ� �Ǿ����ϴ�.", parent->GetGameObject()->GetName(), GetGameObject()->GetName());
		}
		m_Parent = parent;
	

		// ����... ���� ���踦 �����غ��� �Ѵ�.
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

	// parent�� �ڽ��̶� ture�� �����Ѵ�.
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

		// �� ���� ������ �����ؾ� �ϳ�...?
		UpdateTransform();
		m_bChanged = true;
	}
}