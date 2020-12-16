#include "DivePch.h"
#include "Component.h"
#include "Core/Context.h"


namespace Dive
{
	Component::Component(Context * context, GameObject* owner)
		: Object(context),
		m_owner(owner)
	{
		assert(m_owner);
	}
}