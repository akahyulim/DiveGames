#include "DivePch.h"
#include "Model.h"
#include "Material.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Scene/GameObject.h"
#include "Resource/ResourceCache.h"
#include "Resource/Importer/ModelImporter.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace Dive
{
	Model::Model()
		: Resource(eResourceType::Model),
		m_pRootGameObject(nullptr),
		m_pVertexBuffer(nullptr),
		m_pIndexBuffer(nullptr)
	{
	}

	Model::~Model()
	{
		Clear();
		DV_CORE_TRACE("Model 소멸자 호출 - {:s}", GetName());
	}

	bool Model::LoadFromFile(const std::string& filePath)
	{
		if (!FileSystem::FileExists(filePath))
		{
			DV_CORE_ERROR("파일({:s})(이)가 존재하지 않습니다.", filePath);
			return false;
		}

		if (!ResourceCache::GetModelImporter()->Load(this, filePath))
			return false;

		return true;
	}
	
	bool Model::SaveToFile(const std::string& filePath)
	{
		DV_CORE_TRACE("Model::SaveToFile - 현재 모델 구성상태를 '{:s}'파일로 저장", filePath);

		return true;
	}

	void Model::Clear()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);

		if (!m_Vertices.empty())
		{
			m_Vertices.clear();
			m_Vertices.shrink_to_fit();
		}

		if (!m_Indices.empty())
		{
			m_Indices.clear();
			m_Indices.shrink_to_fit();
		}
	}
	
	void Model::AddVertices(std::vector<VertexModel>& vertices, uint32_t* pOffset)
	{
		if (pOffset)
			*pOffset = static_cast<uint32_t>(m_Vertices.size());

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}
	
	void Model::AddIndices(std::vector<uint32_t>& indices, uint32_t* pOffset)
	{
		if (pOffset)
			*pOffset = static_cast<uint32_t>(m_Indices.size());

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end());
	}

	bool Model::CreateBuffers()
	{
		DV_ASSERT(!m_Vertices.empty());
		DV_ASSERT(!m_Indices.empty());

		m_pVertexBuffer = new VertexBuffer;
		if (!m_pVertexBuffer->Create<VertexModel>(m_Vertices))
			return false;

		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
			return false;

		return true;
	}
}