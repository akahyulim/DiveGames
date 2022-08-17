#include "divepch.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	Transform::Transform(Context* pContext)
		: Component(pContext),
		m_pParent(nullptr)
	{
	}

	Transform::~Transform()
	{
		DV_LOG_ENGINE_DEBUG("Transform 소멸자 호출( {0:d}, {1:s})", m_ID, m_pGameObject->GetName());
	}

	void Transform::SetParent(Transform* pParent)
	{
		if (pParent)
		{
			if (pParent->GetID() == this->GetID())
				return;

			if (m_pParent)
			{
				if (m_pParent->GetID() == pParent->GetID())
					return;

				if(pParent->IsChildOf(this))
				{
					// 자신의 자식 노드일 때
					// 새로운 부모의 부모는 현재 자신의 부모
					// 글고 그 새로운 부모에게 자신을 추가
					// 기존 부모로부터 자신은 제거
					// 자신의 부모로 새로운 부모를 추가


					return;
				}
				else
				{
					auto it = m_pParent->m_Children.begin();
					for (it; it != m_pParent->m_Children.end(); ++it)
					{
						if ((*it)->GetID() == this->GetID())
						{
							m_pParent->m_Children.erase(it);
							break;
						}
					}

					m_pParent = pParent;
					pParent->m_Children.emplace_back(this);
				}
			}
			else
			{
				m_pParent = pParent;
				pParent->m_Children.emplace_back(this);
			}
		}
		else
		{
			if(m_pParent)
			{
				auto it = m_pParent->m_Children.begin();
				for (it; it != m_pParent->m_Children.end();++it)
				{
					if ((*it)->GetID() == this->GetID())
					{
						m_pParent->m_Children.erase(it);
						break;
					}
				}
				m_pParent = nullptr;
			}
		}
	}
	
	Transform* Transform::GetRoot()
	{
		if (!m_pParent)
			return this;

		return m_pParent->GetRoot();
	}
	
	Transform* Transform::GetChild(unsigned int index) const
	{
		if(m_Children.empty() || static_cast<unsigned int>(m_Children.size()) < index)
			return nullptr;

		return m_Children[index];
	}
	
	bool Transform::IsChildOf(Transform* pParent) const
	{
		if(!pParent)
			return false;

		if (this->GetID() == pParent->GetID())
			return true;

		if (!m_pParent)
			return false;
		else
			return m_pParent->IsChildOf(pParent);
	}
	
	void Transform::DetachChildren()
	{
		for (auto* pChild : m_Children)
		{
			pChild->SetParent(nullptr);
		}
		m_Children.clear();
		m_Children.shrink_to_fit();
	}

	std::vector<Transform*> Transform::GetChildren() const
	{
		return m_Children;
	}
}