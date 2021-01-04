#include "DivePch.h"
#include "Transform.h"
#include "Scene/Scene.h"
#include "Scene/GameObject.h"


namespace Dive
{
	void Transform::SetParent(Transform * newParent)
	{
		if (!newParent)
		{
			BecomeOrphan();
			return;
		}

		if (this->GetID() == newParent->GetID())
			return;

		if (HasParent())
		{
			if (m_parent->GetID() == newParent->GetID())
				return;
		}

		if(IsDescendant(newParent))
		{
			auto temp = m_children;
			if (HasParent())
			{
				for (const auto& child : temp)
				{
					child->SetParent(m_parent);
				}
			}
			else
			{
				for (const auto& child : temp)
				{
					child->BecomeOrphan();
				}
			}
		}

		auto oldParent = m_parent;
		m_parent = newParent;

		if (oldParent)
			oldParent->AcqurieChildren();
		 
		if(m_parent)
			m_parent->AcqurieChildren();

		updateTransform();
	}

	void Transform::BecomeOrphan()
	{
		if (!m_parent)
			return;

		auto oldParent = m_parent;
		m_parent = nullptr;

		updateTransform();

		oldParent->AcqurieChildren();
	}

	bool Transform::IsAncestor(const Transform * target) const
	{
		if (!HasParent() || !target || !target->HasChildren())
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

	void Transform::GetDescendants(std::vector<Transform*>* descendants)
	{
		if (!descendants)
			return;

		for (auto child : m_children)
		{
			descendants->emplace_back(child);

			if (child->HasChildren())
			{
				child->GetDescendants(descendants);
			}
		}
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

	void Transform::AcqurieChildren()
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
				transform->AcqurieChildren();
			}
		}
	}
}