#include "DivePch.h"
#include "RenderableMesh.h"
#include "../GameObject.h"
#include "Core/Log.h"
#include "Geometry/Mesh.h"
#include "Graphics/Material.h"


namespace Dive
{
	RenderableMesh::RenderableMesh(Context * context, GameObject * owner)
		: Component(context, owner),
		m_mesh(nullptr)
	{
		m_componentType = eComponentType::RenderableMesh;
	}

	RenderableMesh::~RenderableMesh()
	{
	}
	
	bool RenderableMesh::SetMesh(Mesh * mesh)
	{
		if (!mesh || mesh->GetVertexType() != eVertexType::PositionUvNormalTangent)
		{
			CORE_ERROR("�������� �ʰų� �������� �ʴ� Mesh�� ���޹޾ҽ��ϴ�..");
			return false;
		}

		if (m_mesh && mesh)
		{
			if (m_mesh->GetID() == mesh->GetID())
			{
				CORE_TRACE("������ ID({:d})�� Mesh�� ���޹޾ҽ��ϴ�.", m_mesh->GetID());
				return true;
			}
		}

		m_mesh = mesh;

		return true;
	}
}