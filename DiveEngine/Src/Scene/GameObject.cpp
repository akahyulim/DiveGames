#include "GameObject.h"
#include "Scene.h"
#include "../Core/DiveCore.h"
#include "../Helper/Log.h"
#include "../Helper/FileStream.h"

// 모든 Object가 ID를 가질 필요가 있는가?
// Hazle의 경우 Entity만 UUID를 Component로 가진다.

namespace DiveEngine
{
	// 유니티는 이름만 전달 받는다.
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
			pFileStream->Write(GetInstanceID());
			pFileStream->Write(GetName());
		}

		// component
		{
			pFileStream->Write(GetComponentCount());
			for (const auto pComponent : m_Components)
			{
				// eComponentType을 저장토록 수정해야 한다.
				pFileStream->Write(static_cast<unsigned int>(pComponent->GetType()));
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


	// 계층구조 형성 과정에서 이상징후가 포착되었다.
	// SetParent를 두 번 호출하는 것 같다.
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

	// 1. 일단 추가되면 인스펙터에는 보이는데 화면엔 안보인다.
	// 그러다가 다른 걸 추가하거나 수정하면 보인다.
	// 정확하게 말하자면 가장 마지막에 추가한 오브젝트가 안보인다.
	// 2. 계층구조가 만들어지지 않고 있다.
	GameObject* GameObject::Duplicate()
	{
		auto pClone = m_pScene->CreateGameObject();
		pClone->SetName(this->GetName());
		pClone->SetActive(this->IsActive());

		for (auto pOrgComponent : m_Components)
		{
			auto pCloneComponent = pClone->AddComponent(static_cast<unsigned int>(pOrgComponent->GetType()));
			// 일단 이건 먹힌다.
			pCloneComponent->SetAttributes(pOrgComponent->GetAttributes());
		}

		// 스파르탄 코드를 보면 순서가 잘못된 것 같다.
		for (auto pOrgChild : m_pTransform->GetChildren())
		{
			auto pCloneChild = pOrgChild->GetGameObject()->Duplicate();
			CORE_TRACE("Create Clone Child : {:s}", pCloneChild->GetName());
			// 이 과정에서 InstanceID가 동일하다며 리턴한다...
			// 맞다. ID가 중복된다...
			// 일단 SetParent()에서 ID 중복 확인만 주석처리하면 계층구조가 형성된다...
			// 그러고보니 현재 Object에서도 ID를 설정하지 않고 있다...
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

		// 0 말고 다른 건 없을까?
		if (id != 0)
		{
			if(newComponent)
				newComponent->SetInstanceID(id);
		}
		return newComponent;
	}

	// 일단 Scene을 매개변수로 받도록 한다.
	GameObject* GameObject::CreatePrimitive(ePrimitiveType type, Scene* pScene)
	{
		DV_ASSERT(pScene);

		auto pNewGameObject = pScene->CreateGameObject();
		auto pMeshRenderer = pNewGameObject->AddComponent<MeshRenderer>();

		// 이 곳에서 직접 Mesh를 생성하거나
		// 생성된 Mesh를 
		// MeshRenderer에 전달하는 것이 자연스럽다.
		// 결국 Mesh와 Resource를 어떻게 구현하느냐가 중요하다.
		// 그리고 이 과정에서 계층구조를 함께 생각해야만 한다.
		// 또한 이 구조는 ModelImport 과정에도 영향을 미친다.

		switch (type)
		{
		case ePrimitiveType::Plane:
			// Mesh의 Veritces와 Indecies를 CreatePlane에 전달하여 data를 얻은 후 Buffer를 만든다.
			// 이후 MeshRenderer에 Mesh를 전달한다.
			break;
		case ePrimitiveType::Cube:
			break;
		case ePrimitiveType::Sphere:
			break;
		case ePrimitiveType::Capsule:
			break;
		case ePrimitiveType::Cylinder:
			break;


		default:
			pScene->RemoveGameObject(pNewGameObject);
			return nullptr;
		}

		return nullptr;
	}
}