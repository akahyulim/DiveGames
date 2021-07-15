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

	// 현재 부모를 자식으로 삼을 경우 버그가 발생한다.
	void Transform::SetParent(Transform* parent)
	{
		if (parent)
		{
			if (parent->GetInstanceID() == GetInstanceID())
				return;
		}

		// 여러가지 상황이 발생할 수 있다.
		// 예를 들면 자식이 부모가 되거나
		// 자식이 더 상위로 올라가는 것 처럼 말이다.
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

		// 이 둘을 별개로 관리해야 하나...?
		UpdateTransform();
		m_bChanged = true;
	}

	// 이걸 사용했다는 건
	// 결국 부모의 m_Children으로 자식을 관리하는게 아니라
	// 자식의 m_Parent로 m_Children을 재구성한다는 것이다.
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