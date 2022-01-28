#include "divepch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Base/DiveCore.h"
#include "Base/Log.h"
#include "Base/FileStream.h"

// ��� Object�� ID�� ���� �ʿ䰡 �ִ°�?
// Hazle�� ��� Entity�� UUID�� Component�� ������.

namespace Dive
{
	// ����Ƽ�� �̸��� ���� �޴´�.
	GameObject::GameObject(Scene* pScene)
		: m_pTransform(nullptr),
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
			pFileStream->Write(GetID());
			pFileStream->Write(GetName());
		}

		// component
		{
			pFileStream->Write(GetComponentCount());
			for (const auto pComponent : m_Components)
			{
				// eComponentType�� ������� �����ؾ� �Ѵ�.
				pFileStream->Write(static_cast<unsigned int>(pComponent->GetType()));
				pFileStream->Write(pComponent->GetID());
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
				pFileStream->Write(pChild->GetGameObject()->GetID());
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
			SetID(id);
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
				pChild->SetID(pFileStream->ReadAs<unsigned int>());

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

	// 1. �ϴ� �߰��Ǹ� �ν����Ϳ��� ���̴µ� ȭ�鿣 �Ⱥ��δ�.
	// �׷��ٰ� �ٸ� �� �߰��ϰų� �����ϸ� ���δ�.
	// ��Ȯ�ϰ� �����ڸ� ���� �������� �߰��� ������Ʈ�� �Ⱥ��δ�.
	// 2. ���������� ��������� �ʰ� �ִ�.
	GameObject* GameObject::Duplicate()
	{
		auto pClone = m_pScene->CreateGameObject();
		pClone->SetName(this->GetName());
		pClone->SetActive(this->IsActive());

		for (auto pOrgComponent : m_Components)
		{
			auto pCloneComponent = pClone->AddComponent(static_cast<unsigned int>(pOrgComponent->GetType()));
			// �ϴ� �̰� ������.
			pCloneComponent->SetAttributes(pOrgComponent->GetAttributes());
		}

		// ���ĸ�ź �ڵ带 ���� ������ �߸��� �� ����.
		for (auto pOrgChild : m_pTransform->GetChildren())
		{
			auto pCloneChild = pOrgChild->GetGameObject()->Duplicate();
			CORE_TRACE("Create Clone Child : {:s}", pCloneChild->GetName());
			// �� �������� InstanceID�� �����ϴٸ� �����Ѵ�...
			// �´�. ID�� �ߺ��ȴ�...
			// �ϴ� SetParent()���� ID �ߺ� Ȯ�θ� �ּ�ó���ϸ� ���������� �����ȴ�...
			// �׷����� ���� Object������ ID�� �������� �ʰ� �ִ�...
			pCloneChild->GetTransform()->SetParent(pClone->GetTransform());
			CORE_TRACE("Set Hierarchy Parent = {0:s},  Child = {1:s}", pCloneChild->GetTransform()->GetParent()->GetGameObject()->GetName(), pCloneChild->GetName());
		}
		
		return pClone;
	}

	Component* GameObject::AddComponent(unsigned int typeHash, unsigned int id)
	{
		Component* newComponent = nullptr;

		if (eComponentType::Transform == static_cast<eComponentType>(typeHash))
		{
			newComponent = static_cast<Component*>(m_pTransform);
		}
		else if (eComponentType::Camera == static_cast<eComponentType>(typeHash))
		{
			newComponent = m_Components.emplace_back(static_cast<Component*>(new Camera(this)));
		}
		else if (eComponentType::MeshRenderer == static_cast<eComponentType>(typeHash))
		{
			newComponent = m_Components.emplace_back(static_cast<Component*>(new MeshRenderer(this)));
		}
		else if (eComponentType::Light == static_cast<eComponentType>(typeHash))
		{
			newComponent = m_Components.emplace_back(static_cast<Component*>(new Light(this)));
		}

		// 0 ���� �ٸ� �� ������?
		if (id != 0)
		{
			if(newComponent)
				newComponent->SetID(id);
		}
		return newComponent;
	}
}