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

	// ���� �θ� �ڽ����� ���� ��� ���װ� �߻��Ѵ�.
	void Transform::SetParent(Transform* parent)
	{
		if (parent)
		{
			if (parent->GetInstanceID() == GetInstanceID())
				return;
		}

		// �������� ��Ȳ�� �߻��� �� �ִ�.
		// ���� ��� �ڽ��� �θ� �ǰų�
		// �ڽ��� �� ������ �ö󰡴� �� ó�� ���̴�.
		if (HasParent())
		{
			auto sibling = GetParent()->m_Children;
			auto it = sibling.begin();
			for (it; it != sibling.end();)
			{
				if ((*it)->GetInstanceID() == GetInstanceID())
				{
					it = sibling.erase(it);
				}
				else
					it++;
			}
		}

		if(parent)
			parent->m_Children.emplace_back(this);
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
		assert(parent && "Invalid Parameter");
		if (!parent)
		{
			CORE_ERROR("");
			return false;
		}

		if (parent->GetInstanceID() == GetInstanceID())
			return true;

		for (auto child : parent->GetChildren())
		{
			if (child->GetInstanceID() == GetInstanceID())
				return true;
		}

		if (!HasParent())
			return false;

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

	// �̰� ����ߴٴ� ��
	// �ᱹ �θ��� m_Children���� �ڽ��� �����ϴ°� �ƴ϶�
	// �ڽ��� m_Parent�� m_Children�� �籸���Ѵٴ� ���̴�.
	void Transform::AcquireChidren()
	{
		m_Children.clear();
		m_Children.shrink_to_fit();

		// ���߿� Scene�� Global�� �ƴ� �� �ִ�.
		// �׷��ٸ� Scene�� ���޹޾ƾ� �Ѵ�.
		// �ƴϸ� GameObject ������ �ڽ��� ���� Scene�� �����ϵ��� �� ���� �ִ�.
		auto& allGameObjects = SceneManager::GetInstance().GetActiveScene()->GetAllGameObjects();

		// �ᱹ Children�� ��� Clear�� ��
		// �ٽ� GetParent()�� �̿��� ���������� �籸���ϰڴٴ� �ǹ��̴�.
		for (auto gameObject : allGameObjects)
		{
			if (gameObject->GetTransform()->HasParent())
			{
				if (gameObject->GetTransform()->GetParent()->GetInstanceID() == GetInstanceID())
				{
					m_Children.emplace_back(gameObject->GetTransform());
					gameObject->GetTransform()->AcquireChidren();
				}
			}
		}

		// Transform�� Update ���Ѿ� �Ѵ�.
	}
}