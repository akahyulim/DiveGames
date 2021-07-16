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

	// 문제점 정리
	// 0. 파일 로드시 계층구조 과정에서 SetParent가 두 번 호출되는 로그가 찍혔다.
	// => Transform::Deserialize()에서 계층구조를 형성했다.
	// 1. 기존 부모에게서 제거되지 않고 있다.
	// => 참조 문제였다...
	// 2. 여전히 제거 과정에 문제가 발생한다.
	// 3. 현재 부모를 자식으로 삼을 경우 버그가 발생한다.
	void Transform::SetParent(Transform* parent)
	{
		if (parent)
		{
			if (parent->GetInstanceID() == GetInstanceID())
				return;
		}

		// 기존 부모에게서 제거
		if (HasParent())
		{
			// 복사가 아니라 참조로 사용해야 한다.
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

		// 이것두 그냥 이벤트로???
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
		UpdateTransform();
		m_bChanged = true;
	}
}