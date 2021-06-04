#include "GameObject.h"
#include "FileStream.h"
#include "Scene.h"
#include "Log.h"

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code()),
		m_pTransform(nullptr)
	{
		// 하나로 만들까?
		m_pTransform = AddComponent<Transform>();
		m_pTransform->SetOwnder(this);
	}

	void GameObject::Serialize(FileStream* pStream)
	{ 
		if (!pStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			pStream->Write(m_bActive);
			pStream->Write(GetID());
			pStream->Write(m_name);
		}

		// component
		{
			pStream->Write(GetComponentCount());
			for (const auto pComponent : m_components)
			{
				pStream->Write(static_cast<unsigned int>(pComponent->GetTypeHash()));
				pStream->Write(pComponent->GetID());
			}
			for (const auto pComponent : m_components)
			{
				pComponent->Serialize(pStream);
			}
		}

		// children
		{
			auto children = m_pTransform->GetChildren();
			pStream->Write(static_cast<unsigned int>(children.size()));

			for (const auto pChild : children)
			{
				pStream->Write(pChild->GetOwner()->GetID());
			}
			for (const auto pChild : children)
			{
				pChild->GetOwner()->Serialize(pStream);
			}
		}
	}

	void GameObject::Deserialize(FileStream* pStream, Transform* pParent)
	{
		if (!pStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			pStream->Read(&m_bActive);
			unsigned int id;
			pStream->Read(&id);
			SetID(id);
			pStream->Read(&m_name);
		}

		// component
		{
			auto componentCount = pStream->ReadAs<unsigned int>();
			unsigned int type = 0;
			unsigned int id = 0;
			for (unsigned int i = 0; i != componentCount; i++)
			{
				pStream->Read(&type);
				pStream->Read(&id);

				AddComponent(type, id);
			}
			for (const auto pComponent : m_components)
			{
				pComponent->Deserialize(pStream);
			}

			m_pTransform->SetParent(pParent);
		}

		// children
		{
			auto childrenCount = pStream->ReadAs<unsigned int>();
			std::vector<GameObject*> children;
			for (unsigned int i = 0; i != childrenCount; i++)
			{
				auto pChild = Scene::GetGlobalScene().CreateGameObject();
				pChild->SetID(pStream->ReadAs<unsigned int>());

				children.emplace_back(pChild);
			}
			for (const auto pChild : children)
			{
				pChild->Deserialize(pStream, m_pTransform);
			}

			m_pTransform->AcquireChidren();
		}
	}

	void GameObject::Update(float deltaTime)
	{
		for (auto& component : m_components)
		{
			component->Update(deltaTime);
		}
	}

	// 아직 부실하다.
	Component* GameObject::AddComponent(unsigned int typeHash, unsigned int id)
	{
		// Transform은 이미 생성되어 있으므로 id만 바꾼다.
		if (typeid(Transform).hash_code() == typeHash)
		{
			m_pTransform->SetID(id);
			return m_pTransform;
		}

		return nullptr;
	}
}
