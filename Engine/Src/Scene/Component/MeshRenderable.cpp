#include "divepch.h"
#include "MeshRenderable.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
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

	void MeshRenderable::CopyAttributes(Component* pCloneCom)
	{
		if (!pCloneCom)
			return;

		auto pClone = dynamic_cast<MeshRenderable*>(pCloneCom);
		if (!pClone)
			return;

		// 1. Base인 Renderable에서 처리토록 하는 것이 깔끔하다.
		// 2. Material은 동일할 수 도, 개별적으로 가질 수 도 있다. 
		//pClone->m_pMaterial = this->m_pMaterial;
		Renderable::CopyAttributes(pCloneCom);

		pClone->m_GeometryName = this->m_GeometryName;
		pClone->m_pModel = this->m_pModel;
		pClone->m_VertexOffset = this->m_VertexOffset;
		pClone->m_VertexCount = this->m_VertexCount;
		pClone->m_IndexOffset = this->m_IndexOffset;
		pClone->m_IndexCount = this->m_IndexCount;
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
