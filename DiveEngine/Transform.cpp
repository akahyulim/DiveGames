#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"

namespace dive
{
	Transform::Transform()
		: Component(typeid(Transform).hash_code())
	{

	}

	Transform::~Transform()
	{
	}

	void Transform::Update(float deltaTime)
	{
	}

	void Transform::Serialize(FileStream* pStream)
	{
		// local pos, rot ,scl
		// lookAt
		pStream->Write(m_pParent ? m_pParent->GetOwner()->GetID() : 0);
	}

	void Transform::Deserialize(FileStream* pStream)
	{
		// local pos, rot ,scl
		// lookAt
		unsigned int parentId = 0;
		pStream->Read(&parentId);

		if (parentId != 0)
		{
			if (auto pParent = Scene::GetGlobalScene().GetGameObjectByID(parentId))
			{
				SetParent(pParent->GetTransform());
			}
		}

		// update transform
	}

	// 계층 구조 =================================================
	Transform* Transform::GetRoot()
	{
		if (!m_pParent)
			return this;
		else
			return m_pParent->GetRoot();
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (!pParent)
		{
			BecomeOrphan();
			return;
		}

		if (this->GetID() == pParent->GetID())
			return;

		if (m_pParent)
		{
			for (auto it = m_pParent->m_children.begin(); it != m_pParent->m_children.end();)
			{
				if ((*it)->GetID() == this->GetID())
				{
					it = m_pParent->m_children.erase(it);
				}
				else
					++it;
			}
		}

		pParent->m_children.emplace_back(this);
		m_pParent = pParent;
	}

	void Transform::BecomeOrphan()
	{
		if (!m_pParent)
			return;

		auto pOldParent = m_pParent;
		m_pParent = nullptr;

		// UpdateTransform();

		pOldParent->AcquireChidren();
	}

	Transform* Transform::Find(const std::string& name) const
	{
		if (!m_children.empty())
		{
			for (auto& child : m_children)
			{
				if (child->GetOwner() && child->GetOwner()->GetName() == name)
				{
					return child;
				}
			}
		}

		return nullptr;
	}

	Transform* Transform::GetChild(unsigned int index) const
	{
		if (m_children.empty() || m_children.size() - 1 < index)
			return nullptr;

		return m_children[index];
	}

	bool Transform::IsChildOf(const Transform* pParent) const
	{
		if (!pParent)
			return false;

		// pParent이 자신이라도 ture를 리턴한다.
		if (pParent->GetID() == GetID())
			return true;

		if (m_pParent->GetID() == pParent->GetID())
			return true;

		return m_pParent->IsChildOf(pParent);
	}

	void Transform::DetachChildren()
	{
		if (m_children.empty())
			return;

		for (auto pChild : m_children)
		{
			pChild->m_pParent = nullptr;
		}

		m_children.clear();
		m_children.shrink_to_fit();
	}

	void Transform::AcquireChidren()
	{
		m_children.clear();
		m_children.shrink_to_fit();

		auto allGameObjects = Scene::GetGlobalScene().GetAllGameObjects();

		for (auto gameObject : allGameObjects)
		{
			if (gameObject->GetTransform()->HasParent())
			{
				if (gameObject->GetTransform()->GetParent()->GetID() == GetID())
				{
					m_children.emplace_back(gameObject->GetTransform());
					gameObject->GetTransform()->AcquireChidren();
				}
			}
		}
	}
}