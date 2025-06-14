#include "stdafx.h"
#include "Transform.h"
#include "world/GameObject.h"

namespace Dive
{
	Transform::Transform(GameObject* gameObject)
		: Component(gameObject)
	{
		DV_LOG(Transform, trace, "Transform component for GameObject '{}' with InstanceID {} is being created.", gameObject->GetName(), gameObject->GetInstanceID());
	}

	Transform::~Transform()
	{
		DV_LOG(Transform, trace, "Transform component for GameObject '{}' with InstanceID {} is being destroyed.", m_GameObject->GetName(), m_GameObject->GetInstanceID());
	}
	
	void Transform::SetParent(Transform* parent)
	{
		if (parent != nullptr && parent->IsChildOf(this))
			return;

		if (m_Parent != nullptr)
		{
			auto it = std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this);
			if (it != m_Parent->m_Children.end())
				m_Parent->m_Children.erase(it);
		}

		m_Parent = parent;

		if (m_Parent != nullptr)
			m_Parent->m_Children.push_back(this);
	}

	bool Transform::IsChildOf(Transform* parent)
	{
		assert(parent != nullptr);

		if(m_Parent == nullptr)
			return false;

		if (m_Parent == parent)
			return true;
		
		return m_Parent->IsChildOf(parent);
	}

	Transform* Transform::GetChild(size_t index)
	{
		return m_Children.size() > index ? m_Children[index] : nullptr;
	}

	Transform* Transform::Find(const std::string& name)
	{
		for (auto child : m_Children)
		{
			if (child->GetGameObject()->GetName() == name)
				return child;
		}

		return nullptr;
	}

	size_t Transform::GetChildCount()
	{
		return m_Children.size();
	}

	void Transform::DetachChildren()
	{
		for (auto child : m_Children)
		{
			child->m_Parent = nullptr;
		}
		m_Children.clear();
	}

	void Transform::RemoveChild(Transform* child)
	{
		if (!child)
			return;

		auto it = std::find(m_Children.begin(), m_Children.end(), child);
		if (it == m_Children.end())
			return;

		m_Children.erase(it);
	}
}