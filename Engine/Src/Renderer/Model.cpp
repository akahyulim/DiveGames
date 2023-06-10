#include "DivePch.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Scene/GameObject.h"
#include "Scene/Components/Transform.h"
#include "Resource/ResourceCache.h"
#include "Resource/Importer/AssetImporter.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

#include <yaml-cpp/yaml.h>

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

		m_FilePath = filePath;

		return true;
	}
	
	bool Model::SaveToFile(const std::string& filePath)
	{
		if (!m_pRootGameObject)
		{
			DV_CORE_ERROR("Root GameObject가 존재하지 않아 파일 저장에 실패하였습니다.");
			return false;
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "type" << YAML::Value << "model";
		out << YAML::EndMap;

		// 실제로는 생성된 파일의 스트림을 전달
		//m_pRootGameObject->SaveToFile(filePath);
		m_pRootGameObject->SaveToYAML(out);

		// 직접 mesh 정보 저장?
		// urho의 경우 model에서 저장했다.
		for (IMesh* pMesh : m_Meshes)
		{
			// 할려면 구체 타입으로 변환한 뒤 구현을 나누어야 한다.
		}

		std::ofstream fout(filePath);
		fout << out.c_str();

		return true;
	}

	void Model::Clear()
	{
		DV_DELETE(m_pIndexBuffer);
		DV_DELETE(m_pVertexBuffer);

		//DV_DELETE(m_pMesh);

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
	
	void Model::AddVertices(std::vector<VertexStatic>& vertices, uint32_t* pOffset)
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
		if (!m_pVertexBuffer->Create<VertexStatic>(m_Vertices))
			return false;

		m_pIndexBuffer = new IndexBuffer;
		if (!m_pIndexBuffer->Create<uint32_t>(m_Indices))
			return false;

		return true;
	}
}