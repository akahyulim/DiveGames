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

	// =============================================//
	// 상위 노드의 부모를 하위 노드로 변경할 경우	//
	// 모두 사라지는 버그가 있다.					//
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

		// 이 재귀함수는 무거워 보이는데
		// 굳이 이렇게 할 필요가 있나?
		// 그냥 부모의 children에서 자신만 지우면 되는데..
		// 그러고보니 removeChild같은게 없네.
		oldParent->AcquireChidren();
	}

	//==================================//
	// GameObject의 이름을 사용합니다.	//
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

	// parent가 자신이라도 ture를 리턴한다.
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

		// 나중에 Scene이 Global이 아닐 수 있다.
		// 그렇다면 Scene을 전달받아야 한다.
		// 아니면 GameObject 측에서 자신이 속한 Scene을 저장하도록 할 수도 있다.
		auto& allGameObjects = SceneManager::GetInstance().GetActiveScene()->GetAllGameObjects();

		// 결국 Children을 모두 Clear한 후
		// 다시 GetParent()를 이용해 계층구조를 재구축하겠다는 의미이다.
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

		// Transform도 Update 시켜야 한다.
	}
}