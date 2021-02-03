#include "DivePch.h"
#include "Renderable.h"
#include "Scene/GameObject.h"
#include "Core/Log.h"
#include "Graphics/Material.h"


namespace Dive
{
	Renderable::Renderable(Context * context, GameObject * owner)
		: Component(context, owner),
		m_vertexBufferOffset(0),
		m_indexBufferOffset(0),
		m_material(nullptr)
	{
		m_type = eComponentType::Renderable;
		m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		CORE_TRACE("Create {:s}'s Renderable Component.", m_owner->GetName());

	}

	Renderable::~Renderable()
	{
		CORE_TRACE("Destroy {:s}'s Renderable Component.", m_owner->GetName());
	}

	void Renderable::SetMaterial(Material * material)
	{
		// Mesh(Vertex Type)�� ���� ���ѵǾ�� �Ѵ�.
		// �ϴ��� �׳� ����.
		m_material = material;
	}
}