#include "DivePch.h"
#include "Drawable.h"
#include "Renderer/Model.h"
#include "Renderer/Material.h"
#include "Resource/ResourceCache.h"
#include "IO/Log.h"

namespace Dive
{
	Drawable::Drawable(GameObject* pGameObject)
		: Component(pGameObject),
		m_pModel(nullptr),
		m_MeshVertexOffset(0),
		m_MeshVertexCount(0),
		m_MeshIndexOffset(0),
		m_MeshIndexCount(0),
		m_pMaterial(nullptr)
	{
	}

	Drawable::~Drawable()
	{
	}

	void Drawable::SetMesh(Model* pModel, const std::string& name, uint32_t vertexOffset, uint32_t vertexCount, uint32_t indexOffset, uint32_t indexCount)
	{
		m_pModel = pModel;
		m_Name = m_MeshName = name;
		m_MeshVertexOffset = vertexOffset;
		m_MeshVertexCount = vertexCount;
		m_MeshIndexOffset = indexOffset;
		m_MeshIndexCount = indexCount;
	}

	void Drawable::SetMaterial(const std::string& filePath)
	{
		m_pMaterial = ResourceCache::GetResourceByPath<Material>(filePath);
	}
}