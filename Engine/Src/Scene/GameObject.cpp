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
			m_name = "GameObject";
		else
			m_name = std::move(name);

		// transform은 기본적으로 추가한다.
		AddComponent<Transform>();

		CORE_TRACE("Create GameObject - {:s}.", m_name);
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		CORE_TRACE("Destroy GameObject - {:s}", m_name);
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