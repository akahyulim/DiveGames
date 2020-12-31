#include "DivePch.h"
#include "Component.h"
#include "../GameObject.h"
#include "Core/Context.h"


namespace Dive
{
	Component::Component(Context * context, GameObject* owner)
		: Object(context),
		m_owner(owner)
	{
		assert(m_owner);
	}

	std::string Component::GetName() const
	{
		if(!m_owner)
			return std::string();

		return m_owner->GetName();
	}
}