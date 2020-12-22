#include "DivePch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"
#include "Graphics/Model.h"
#include "Graphics/Material.h"


namespace Dive
{
	Renderable::Renderable(Context * context, GameObject * owner)
		: Component(context, owner)
	{
		m_type = eComponentType::Renderable;
		m_model = std::make_shared<Model>(m_context);
	}

	Renderable::~Renderable()
	{
	}
}