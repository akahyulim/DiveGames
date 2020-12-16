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

		// id�� �Բ� �����ϴ°� ���� �� ��︰��.

		CORE_TRACE("Create GameObject - {:s}.", m_name);
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		CORE_TRACE("Destroy GameObject.");
	}

	void GameObject::Clone()
	{
		// ���� ���� ����
		// �׷��� World�� ����Ѵ�.
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