#include "divepch.h"
#include "GameObject.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	GameObject::GameObject(Context* pContext)
		: Object(pContext),
		m_pScene(nullptr),
		m_ID(0)
	{
	}

	GameObject::~GameObject()
	{
		DV_LOG_ENGINE_DEBUG("GameObject( {0:s} : {1:d}) 소멸자 호출", m_Name, m_ID);
	}
}