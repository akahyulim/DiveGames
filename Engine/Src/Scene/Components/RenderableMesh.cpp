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
			CORE_ERROR("존재하지 않거나 지원하지 않는 Mesh를 전달받았습니다..");
			return false;
		}

		if (m_mesh && mesh)
		{
			if (m_mesh->GetID() == mesh->GetID())
			{
				CORE_TRACE("동일한 ID({:d})의 Mesh를 전달받았습니다.", m_mesh->GetID());
				return true;
			}
		}

		m_mesh = mesh;

		return true;
	}
}