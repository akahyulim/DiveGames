#include "divepch.h"
#include "GameObject.h"
#include "Scene.h"
#include "Base/Base.h"

namespace Dive
{
	GameObject::GameObject(Scene* pScene, const std::string& name, unsigned long long id)
		: m_pScene(pScene), Object(name, id)
	{
	}

	GameObject::~GameObject()
	{
		if (m_Components.empty())
			return;

		for(auto pComponent : m_Components)
		{
			DV_DELETE(pComponent);
		}
		m_Components.clear();
	}
}