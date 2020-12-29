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

		// transform�� �⺻������ �߰��Ѵ�.
		AddComponent<Transform>();

		CORE_TRACE("Create GameObject - {:s}, {:d}.", m_name, m_ID);
	}

	GameObject::~GameObject()
	{
		m_components.clear();

		CORE_TRACE("Destroy GameObject - {:s}, {:d}", m_name, m_ID);
	}

	void GameObject::Clone()
	{
		// ���� ���� ����
		// component�� ����
		// �������� ���� �����ؾ� �ϱ⿡ �����ϴ�.
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