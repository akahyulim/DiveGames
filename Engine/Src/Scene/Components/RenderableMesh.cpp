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

	Material * RenderableMesh::GetMaterial(size_t index) const
	{
		if(m_materials.empty() || index >= m_materials.size())
			return nullptr;

		return m_materials[index];
	}

	bool RenderableMesh::AddMaterial(Material * material)
	{
		if(!material)	return false;

		for (const auto& mat : m_materials)
		{
			if (mat->GetID() == material->GetID())
				return false;
		}

		m_materials.emplace_back(material);

		return true;
	}
}