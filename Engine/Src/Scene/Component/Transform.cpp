#include "divepch.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Core/Context.h"
#include "IO/Log.h"

namespace Dive
{
	Transform::Transform(Context* pContext)
		: Component(pContext)
	{

	}

	Transform::~Transform()
	{
		DV_LOG_ENGINE_DEBUG("Transform ¼Ò¸êÀÚ È£Ãâ( {0:d}, {1:d})", m_ID, m_pGameObject->GetID());
	}
}