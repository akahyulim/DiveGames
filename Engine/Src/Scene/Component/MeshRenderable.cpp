#include "divepch.h"
#include "MeshRenderable.h"
#include "Renderer/Mesh.h"
#include "Renderer/LegacyMaterial.h"
#include "Renderer/Model.h"

namespace Dive
{
	MeshRenderable::MeshRenderable(GameObject* pGameObject)
		: Renderable(pGameObject)
	{
	}

	MeshRenderable::~MeshRenderable()
	{
	}
	
	void MeshRenderable::SetGeometry(const std::string& name, unsigned int vertexOffset, unsigned int vertexCount, unsigned int indexOffset, unsigned int indexCount, Model* pModel)
	{
		m_GeometryName	= name;
		m_VertexOffset	= vertexOffset;
		m_VertexCount	= vertexCount;
		m_IndexOffset	= indexOffset;
		m_IndexCount	= indexCount;
		m_pModel		= pModel;
	}
}
