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
		// ���� ������ �ƴϴ�. �߸��ߴ�.
		m_model = new Model(m_context);//std::make_shared<Model>(m_context);
		m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	Renderable::~Renderable()
	{
		
	}
}