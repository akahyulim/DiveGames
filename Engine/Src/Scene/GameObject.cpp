#include "DivePch.h"
#include "GameObject.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "Components/Component.h"

namespace Dive
{
	GameObject::GameObject(Context * context, std::string name)
		: Object(context),
		m_bActive(true)
	{
		if (name.empty())
			m_name = "GameObject";
		else
			m_name = std::move(name);

		// id도 함께 생성하는게 가장 잘 어울린다.

		CORE_TRACE("Create GameObject - {:s}.", m_name);
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		CORE_TRACE("Destroy GameObject.");
	}

	void GameObject::Clone()
	{
		// 깊은 복사 수행
		// 그런데 World에 등록한다.
	}
	
	void GameObject::Start()
	{
		CORE_TRACE("Start GameObject.");

		for (auto& component : m_components)
			component->OnStart();
	}
	
	void GameObject::Stop()
	{
		CORE_TRACE("Stop GameObject.");

		for (auto& component : m_components)
			component->OnStop();
	}
	
	void GameObject::Update()
	{
		if (!m_bActive)
			return;

		CORE_TRACE("Update GameObject.");

		for (auto& component : m_components)
			component->OnUpdate();
	}
}