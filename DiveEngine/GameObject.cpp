#include "GameObject.h"
#include "FileStream.h"
#include "Scene.h"
#include "Log.h"

namespace dive
{
	GameObject::GameObject(Scene* pScene)
		: Object(typeid(GameObject).hash_code()),
		m_pTransform(nullptr),
		m_pScene(pScene)
	{
		SetName("GameObject");
		m_pTransform = AddComponent<Transform>();
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

	void GameObject::Serialize(FileStream* pFileStream)
	{ 
		if (!pFileStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			pFileStream->Write(m_bActive);
			pFileStream->Write(GetInstanceID());
			pFileStream->Write(GetName());
		}

		// component
		{
			pFileStream->Write(GetComponentCount());
			for (const auto pComponent : m_Components)
			{
				pFileStream->Write(static_cast<unsigned int>(pComponent->GetTypeHash()));
				pFileStream->Write(pComponent->GetInstanceID());
			}
			for (const auto pComponent : m_Components)
			{
				pComponent->Serialize(pFileStream);
			}
		}

		// children
		{
			auto children = m_pTransform->GetChildren();
			pFileStream->Write(static_cast<unsigned int>(children.size()));

			for (const auto pChild : children)
			{
				pFileStream->Write(pChild->GetGameObject()->GetInstanceID());
			}
			for (const auto pChild : children)
			{
				pChild->GetGameObject()->Serialize(pFileStream);
			}
		}
	}


	// �������� ���� �������� �̻�¡�İ� �����Ǿ���.
	// SetParent�� �� �� ȣ���ϴ� �� ����.
	void GameObject::Deserialize(FileStream* pFileStream, Transform* pParentTransform)
	{
		if (!pFileStream)
		{
			CORE_ERROR("");
			return;
		}

		// basic
		{
			pFileStream->Read(&m_bActive);
			unsigned int id;
			pFileStream->Read(&id);
			SetInstanceID(id);
			std::string name;
			pFileStream->Read(&name);
			SetName(name);
		}

		// component
		{
			auto componentCount = pFileStream->ReadAs<unsigned int>();
			unsigned int type = 0;
			unsigned int id = 0;
			for (unsigned int i = 0; i != componentCount; i++)
			{
				pFileStream->Read(&type);
				pFileStream->Read(&id);

				AddComponent(type, id);
			}
			for (const auto pComponent : m_Components)
			{
				pComponent->Deserialize(pFileStream);
			}

			m_pTransform->SetParent(pParentTransform);
		}

		// children
		{
			auto childrenCount = pFileStream->ReadAs<unsigned int>();
			std::vector<GameObject*> children;
			for (unsigned int i = 0; i != childrenCount; i++)
			{
				auto pChild = GetScene()->CreateGameObject(true);
				pChild->SetInstanceID(pFileStream->ReadAs<unsigned int>());

				children.emplace_back(pChild);
			}
			for (const auto pChild : children)
			{
				pChild->Deserialize(pFileStream, m_pTransform);
			}
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

	//======================================================================//
	// ���Ͽ� unsigned int�� �����߱⿡ typid.hash_code�� ����ȯ �ؾ� �Ѵ�.	//
	// id �κе� ���� ������ ���� �ʴ´�.									//
	//======================================================================//
	Component* GameObject::AddComponent(unsigned int typeHash, unsigned int id)
	{
		Component* newComponent = nullptr;

		if ((unsigned int)typeid(Transform).hash_code() == typeHash)
		{
			newComponent = static_cast<Component*>(m_pTransform);
		}
		else if ((unsigned int)typeid(Camera).hash_code() == typeHash)
		{
			newComponent = m_Components.emplace_back(static_cast<Component*>(new Camera(this)));
		}
		else if ((unsigned int)typeid(MeshRenderer).hash_code() == typeHash)
		{
			newComponent = m_Components.emplace_back(static_cast<Component*>(new MeshRenderer(this)));
		}

		// ���� �������� ���� �κ��� �־ �ϴ� �ּ�ó��
		//assert(newComponent);

		// 0 ���� �ٸ� �� ������?
		if (id != 0)
		{
			if(newComponent)
				newComponent->SetInstanceID(id);
		}
		return newComponent;
	}
}
