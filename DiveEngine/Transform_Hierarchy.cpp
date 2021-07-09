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

	// =============================================//
	// ���� ����� �θ� ���� ���� ������ ���	//
	// ��� ������� ���װ� �ִ�.					//
	//==============================================//
	void Transform::SetParent(Transform* parent)
	{
		if (!parent)
		{
			BecomeOrphan();
			return;
		}

		if (this->GetInstanceID() == parent->GetInstanceID())
			return;

		if (m_Parent)
		{
			for (auto it = m_Parent->m_Children.begin(); it != m_Parent->m_Children.end();)
			{
				if ((*it)->GetInstanceID() == this->GetInstanceID())
				{
					it = m_Parent->m_Children.erase(it);
				}
				else
					++it;
			}
		}

		parent->m_Children.emplace_back(this);
		m_Parent = parent;
	}

	void Transform::BecomeOrphan()
	{
		if (!m_Parent)
			return;

		auto oldParent = m_Parent;
		m_Parent = nullptr;

		UpdateTransform();

		// �� ����Լ��� ���ſ� ���̴µ�
		// ���� �̷��� �� �ʿ䰡 �ֳ�?
		// �׳� �θ��� children���� �ڽŸ� ����� �Ǵµ�..
		// �׷����� removeChild������ ����.
		oldParent->AcquireChidren();
	}

	//==================================//
	// GameObject�� �̸��� ����մϴ�.	//
	//==================================//
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
			return false;

		if (parent->GetInstanceID() == GetInstanceID())
			return true;

		if (m_Parent->GetInstanceID() == parent->GetInstanceID())
			return true;

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

		UpdateTransform();
	}

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