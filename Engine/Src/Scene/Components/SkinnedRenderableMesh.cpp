#include "DivePch.h"
#include "SkinnedRenderableMesh.h"
#include "Core/Log.h"
#include "Geometry/Mesh.h"


namespace Dive
{
	SkinnedRenderableMesh::SkinnedRenderableMesh(Context * context, GameObject * owner)
		: RenderableMesh(context, owner)
	{
		m_componentType = eComponentType::SkinnedRenderableMesh;
	}

	SkinnedRenderableMesh::~SkinnedRenderableMesh()
	{
	}
	
	bool SkinnedRenderableMesh::SetMesh(Mesh * mesh)
	{
		// 지원하는 type인지 확인해야 한다.

		if (m_mesh && mesh)
		{
			if (m_mesh->GetID() == mesh->GetID())
			{
				CORE_ERROR("");
				return true;
			}
		}

		m_mesh = mesh;

		return true;
	}
}