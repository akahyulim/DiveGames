#include "divepch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Base/Base.h"

namespace Dive
{
	GameObject::GameObject(Scene* pScene, const std::string& name)
		: m_pScene(pScene), Object(name)
	{
		DV_ASSERT(m_pScene != nullptr);

		DV_CORE_TRACE("GameObject InstanceID: {:d}", GetInstanceID());
	}

	GameObject::GameObject(Scene* pScene, unsigned long long id, const std::string& name)
		: m_pScene(pScene), Object(id, name)
	{
		DV_ASSERT(m_pScene != nullptr);

		DV_CORE_TRACE("Copied GameObject InstanceID: {:d}", GetInstanceID());
	}

	GameObject::~GameObject()
	{
		DV_CORE_TRACE("Destroy {0:s}: {1:d}", m_Name, m_InstanceID);

		if (m_Components.empty())
			return;

		for(auto pComponent : m_Components)
		{
			DV_DELETE(pComponent);
		}
		m_Components.clear();
	}
}