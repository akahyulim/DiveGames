#include "DivePch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Core/Context.h"
#include "Core/FileStream.h"
#include "Core/Log.h"
#include "Components/Renderable.h"
#include "Components/Transform.h"


namespace Dive
{
	GameObject::GameObject(Context * context, std::string name)
		: Object(context),
		m_bActive(true),
		m_bPendingDestruction(false)
	{
		m_name = name.empty() ? "Game Object" : std::move(name);

		m_transform = AddComponent<Transform>();

		CORE_TRACE("Create GameObject - {:s}, {:d}.", m_name, m_ID);
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		CORE_TRACE("Destroy GameObject - {:s}, {:d}", m_name, m_ID);
	}

	void GameObject::Serialize(FileStream& stream)
	{
		// basic data
		{
			stream.Write(m_bActive);
			stream.Write(m_ID);
			stream.Write(m_name);
		}

		// components
		{
			stream.Write(GetComponentsCount());

			for (const auto& component : m_components)
			{
				stream.Write(static_cast<unsigned int>(component->GetType()));
				stream.Write(component->GetID());
			}

			for (const auto& component : m_components)
			{
				component->Serialize(stream);
			}
		}

		// children
		{
			auto children = m_transform->GetChildren();
			stream.Write(static_cast<unsigned int>(children.size()));

			for (const auto& child : children)
			{
				stream.Write(child->GetOwner()->GetID());
			}

			for (const auto& child : children)
			{
				child->GetOwner()->Serialize(stream);
			}
		}
	}

	void GameObject::Deserialize(FileStream & stream, Transform * parent)
	{
		// basic data
		{
			stream.Read(&m_bActive);
			stream.Read(&m_ID);
			stream.Read(&m_name);
		}

		// components
		{
			auto componentCount = stream.ReadAs<unsigned int>();
			unsigned int type = 0;
			unsigned int id = 0;
			for (unsigned int i = 0; i != componentCount; i++)
			{
				stream.Read(&type);
				stream.Read(&id);

				AddComponent(static_cast<eComponentType>(type), id);
			}

			for (const auto& component : m_components)
			{
				component->Deserialize(stream);
			}

			m_transform->SetParent(parent);
		}

		// children
		{
			auto childrenCount = stream.ReadAs<unsigned int>();
			
			auto scene = GetSubsystem<Scene>();
			std::vector<std::weak_ptr<GameObject>> children;
			for (unsigned int i = 0; i != childrenCount; i++)
			{
				auto child = scene->CreateGameObject();
				child->SetID(stream.ReadAs<unsigned int>());

				children.emplace_back(child);
			}

			for (const auto& child : children)
			{
				child.lock()->Deserialize(stream, m_transform);
			}

			m_transform->AcqurieChildren();
		}
	}

	GameObject* GameObject::Clone()
	{
		auto scene = GetSubsystem<Scene>();

		auto clone = scene->CreateGameObject();
		clone->SetName(m_name);
		clone->SetActive(m_bActive);
	
		for (auto component : m_components)
		{
			auto cloneComponent = clone->AddComponent(component->GetType());
			cloneComponent->Copy(component.get());
		}
	
		for (auto child : m_transform->GetChildren())
		{
			auto cloneChild = child->GetOwner()->Clone();
			cloneChild->SetParentByGameObject(clone.get());
		}

		return clone.get();
	}
	
	void GameObject::Start()
	{
		for (auto& component : m_components)
			component->OnStart();
	}
	
	void GameObject::Stop()
	{
		for (auto& component : m_components)
			component->OnStop();
	}
	
	void GameObject::Update()
	{
		if (!m_bActive)
			return;

		for (auto& component : m_components)
			component->OnUpdate();
	}

	Component * GameObject::AddComponent(eComponentType type, unsigned int id)
	{
		switch (type)
		{
		case eComponentType::Renderable:
		{
			auto component = AddComponent<Renderable>();
			if(id != 0 ) component->SetID(id);
			return static_cast<Component*>(component);
		}

		case eComponentType::Transform:
		{
			auto component = AddComponent<Transform>();
			if (id != 0) component->SetID(id);
			return static_cast<Component*>(component);
		}

		default:
			return nullptr;
		}
	}

	GameObject * GameObject::GetParentGameObject()
	{
		auto parentTransform = m_transform->GetParent();
		if(!parentTransform)
			return nullptr;

		return parentTransform->GetOwner();
	}

	void GameObject::SetParentByGameObject(GameObject * newParent)
	{
		if (!newParent)
			return;

		SetParentByTrasnsform(newParent->GetTransform());
	}

	void GameObject::SetParentByTrasnsform(Transform * newParent)
	{
		if (!newParent)
			return;

		m_transform->SetParent(newParent);
	}
}