#include "DivePch.h"
#include "GameObject.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Components/Component.h"
#include "Components/Transform.h"

namespace Dive
{
	GameObject::GameObject(Context * context, std::string name)
		: Object(context),
		m_bActive(true),
		m_bPendingDestruction(false)
	{
		if (name.empty())
			m_name = "Empty";
		else
			m_name = std::move(name);

		// transform은 기본적으로 추가한다.
		AddComponent<Transform>();

		CORE_TRACE("Create GameObject - {:s}, {:d}.", m_name, m_ID);
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		CORE_TRACE("Destroy GameObject - {:s}, {:d}", m_name, m_ID);
	}

	void GameObject::Serialize(void * stream)
	{
		// basic data
		{
			// is active
			// hierarchy visibility => editor camera를 숨기는 것과 같은 기능?
			// id
			// name
		}

		// components
		{
			// component 개수
			// component type id
			// compoennt id

			// component serialize
		}

		// children
		{
			// children 획득

			// children count

			// children id

			// 모든 children의 ower를 통해 serialize
		}
	}

	void GameObject::Deserialize(void * stream, Transform * parent)
	{
		// basic data
		{
			// is active
			// hierarchy visibility => editor camera를 숨기는 것과 같은 기능?
			// id
			// name
		}

		// components
		{
			// component 개수
			// component type id
			// compoennt id
			// 들을 통해 component 생성 => 생성 함수 추가 구현 필요

			// component deserialize

			// 전달받은 parent를 set parent
		}

		// children
		{
			// children count
			// scene을 통해 child를 count만큼 생성하며 id 전달

			// children에서 child들의 deserialize

			// transform의 acquireChildren => 부모를 통해 계층관계 형성
		}
	}

	void GameObject::Clone()
	{
		// 깊은 복사 수행
		// component는 물론
		// 계층구조 까지 염두해야 하기에 복잡하다.
	}
	
	void GameObject::Start()
	{
		//CORE_TRACE("Start GameObject.");

		for (auto& component : m_components)
			component->OnStart();
	}
	
	void GameObject::Stop()
	{
		//CORE_TRACE("Stop GameObject.");

		for (auto& component : m_components)
			component->OnStop();
	}
	
	void GameObject::Update()
	{
		if (!m_bActive)
			return;

		//CORE_TRACE("Update GameObject.");

		for (auto& component : m_components)
			component->OnUpdate();
	}
}