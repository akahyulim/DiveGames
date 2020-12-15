#include "DivePch.h"
#include "Component.h"
#include "Core/Context.h"
#include "Scene/GameObject.h"


namespace Dive
{
	Component::Component(Context * context, GameObject* owner)
		: Object(context),
		m_owner(owner)
	{
	}
}