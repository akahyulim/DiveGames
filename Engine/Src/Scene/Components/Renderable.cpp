#include "DivePch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"


namespace Dive
{
	Renderable::Renderable(Context * context, GameObject * owner)
		: Component(context, owner)
	{
		m_type = eComponentType::Renderable;
	}

	Renderable::~Renderable()
	{
	}
}