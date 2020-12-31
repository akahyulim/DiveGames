#include "DivePch.h"
#include "Transform.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"


namespace Dive
{
	void Transform::SetParent(Transform * parent)
	{
		// 빈 객체를 전달받으면 독립
		if (!parent)
		{
			BecomeOrphan();
			return;
		}

		// 자신을 부모를 삼으려는 경우
		if (GetID() == parent->GetID())
			return;

		// 동일한 부모를 전달받은 경우
		if (HasParent())
		{
			if (m_parent->GetID() == parent->GetID())
				return;
		}

		// 자손을 전달받은 경우
		if (parent->IsAncestor(this))
		{
			if (HasParent())
			{
				for (const auto& child : m_children)
				{
					child->SetParent(m_parent);
				}
			}
			//
			else
			{
				for (const auto& child : m_children)
				{
					child->BecomeOrphan();
				}
			}
		}

		auto oldParent = m_parent;
		m_parent = parent;

		if (oldParent)
			oldParent->AcquireChildern();

		if(m_parent)
			m_parent->AcquireChildern();

		updateTransform();
	}

	void Transform::BecomeOrphan()
	{
		if (!m_parent)
			return;

		auto oldParent = m_parent;
		m_parent = nullptr;

		updateTransform();

		// 굳이 이럴 필요가 있나?
		// 오버헤드 같은데...
		oldParent->AcquireChildern();
	}

	bool Transform::IsAncestor(const Transform * target) const
	{
		if (!target || !HasParent() || !target->HasChildren())
			return false;

		for (const auto& child : target->GetChildren())
		{
			if (GetID() == child->GetID())
				return true;

			if (child->HasChildren())
			{
				if (IsAncestor(child))
					return true;
			}
		}

		return false;
	}

	bool Transform::IsDescendant(const Transform * target) const
	{
		if (!target || !target->HasParent())
			return false;

		if (GetID() == target->GetID())
			return false;

		for (const auto& child : m_children)
		{
			if (child->GetID() == target->GetID())
				return true;

			if (child->HasChildren())
			{
				if (child->IsDescendant(target))
					return true;
			}
		}

		return false;
	}

	void Transform::AddChild(Transform * child)
	{
		if (!child || GetID() == child->GetID())
			return;

		child->SetParent(this);
	}

	Transform * Transform::GetRoot()
	{
		if (!m_parent)
			return this;

		return m_parent->GetRoot();
	}

	Transform * Transform::GetChildByName(const std::string & name) const
	{
		if (m_children.empty() || name.empty())
			return nullptr;

		for (const auto& child : m_children)
		{
			if (child->GetName() == name)
				return child;
		}

		return nullptr;
	}

	Transform * Transform::GetChildByIndex(unsigned int index) const
	{
		if (m_children.empty() || index > m_children.size())
			return nullptr;

		return m_children[index];
	}

	void Transform::AcquireChildern()
	{
		m_children.clear();
		m_children.shrink_to_fit();

		auto gameObjects = GetSubsystem<Scene>()->GetAllGameObjects();
		for (const auto& gameObject : gameObjects)
		{
			auto transform = gameObject->GetComponent<Transform>();
			if (!transform->HasParent())
				continue;

			if (transform->GetParent()->GetID() == this->GetID())
			{
				m_children.emplace_back(transform);
				transform->AcquireChildern();
			}
		}
	}
}