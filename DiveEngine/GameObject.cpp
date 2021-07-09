#include "GameObject.h"
#include "FileStream.h"
#include "Scene.h"
#include "Log.h"

namespace dive
{
	GameObject::GameObject(Scene* scene)
		: Object(typeid(GameObject).hash_code()),
		m_Transform(nullptr),
		m_Scene(scene)
	{
		SetName("GameObject");
		m_Transform = AddComponent<Transform>();
	}

	GameObject::~GameObject()
	{
		if (!m_Components.empty())
		{
			for (auto component : m_Components)
			{
				delete component;
				component = nullptr;
			}

			m_Components.clear();
		}

		APP_TRACE("Destroy GameObject: {:s}", GetName());
	}

	void GameObject::Serialize(FileStream* fileStream)
	{ 
		if (!fileStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			fileStream->Write(m_bActive);
			fileStream->Write(GetInstanceID());
			fileStream->Write(GetName());
		}

		// component
		{
			fileStream->Write(GetComponentCount());
			for (const auto pComponent : m_Components)
			{
				fileStream->Write(static_cast<unsigned int>(pComponent->GetTypeHash()));
				fileStream->Write(pComponent->GetInstanceID());
			}
			for (const auto pComponent : m_Components)
			{
				pComponent->Serialize(fileStream);
			}
		}

		// children
		{
			auto children = m_Transform->GetChildren();
			fileStream->Write(static_cast<unsigned int>(children.size()));

			for (const auto pChild : children)
			{
				fileStream->Write(pChild->GetGameObject()->GetInstanceID());
			}
			for (const auto pChild : children)
			{
				pChild->GetGameObject()->Serialize(fileStream);
			}
		}
	}

	void GameObject::Deserialize(FileStream* fileStream, Transform* parentTransform)
	{
		if (!fileStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			fileStream->Read(&m_bActive);
			unsigned int id;
			fileStream->Read(&id);
			SetInstanceID(id);
			std::string name;
			fileStream->Read(&name);
			SetName(name);
		}

		// component
		{
			auto componentCount = fileStream->ReadAs<unsigned int>();
			unsigned int type = 0;
			unsigned int id = 0;
			for (unsigned int i = 0; i != componentCount; i++)
			{
				fileStream->Read(&type);
				fileStream->Read(&id);

				AddComponent(type, id);
			}
			for (const auto pComponent : m_Components)
			{
				pComponent->Deserialize(fileStream);
			}

			m_Transform->SetParent(parentTransform);
		}

		// children
		{
			auto childrenCount = fileStream->ReadAs<unsigned int>();
			std::vector<GameObject*> children;
			for (unsigned int i = 0; i != childrenCount; i++)
			{
				auto pChild = GetScene()->CreateGameObject(true);
				pChild->SetInstanceID(fileStream->ReadAs<unsigned int>());

				children.emplace_back(pChild);
			}
			for (const auto pChild : children)
			{
				pChild->Deserialize(fileStream, m_Transform);
			}

			m_Transform->AcquireChidren();
		}

		EVENT_FIRE(eEventType::SceneResolve);
	}

	void GameObject::Update(float deltaTime)
	{
		for (auto& component : m_Components)
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
			m_Transform->SetInstanceID(id);
			return m_Transform;
		}

		return nullptr;
	}
}
