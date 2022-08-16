#include "divepch.h"
#include "Component.h"
#include "Core/Context.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"

namespace Dive
{
	Component::Component(Context* pContext)
		: Object(pContext),
		m_pGameObject(nullptr),
		m_ID(0)
	{
	}

	void Component::setGameObject(GameObject* pGameObject)
	{
		m_pGameObject = pGameObject;
	}

	void Component::setID(unsigned int id)
	{
		m_ID = id;
	}
}