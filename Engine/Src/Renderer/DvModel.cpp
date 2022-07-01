#include "divepch.h"
#include "DvModel.h"
#include "DvMesh.h"	
#include "Base/Base.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/DvIndexBuffer.h"

namespace Dive
{
	// 엔진 포멧을 받은 후 구성
	bool DvModel::LoadFromFile(const std::string& filepath)
	{
		return false;
	}

	bool DvModel::SaveToFile(const std::string& filepath)
	{
		return false;
	}

	// 파일화 테스트
	void DvModel::Save()
	{
		// write vertex buffers
		// buffers count
		DV_CORE_INFO("VertexBuffer Count: {:d}", m_VertexBuffers.size());
		for (size_t i = 0; i < m_VertexBuffers.size(); ++i)
		{
			auto pBuffer = m_VertexBuffers[i];
			// verex count
			DV_CORE_INFO("Vertex Count: {:d}", pBuffer->GetCount());
			// element size
			// element desc
			// start
			// count: 위의 count와는 다른 듯...
		}

		// write index buffers
		// buffers size
		DV_CORE_INFO("IndexBuffer Count: {:d}", m_IndexBuffers.size());
		for (size_t i = 0; i < m_IndexBuffers.size(); ++i)
		{
			auto pBuffer = m_IndexBuffers[i];
			// count
			DV_CORE_INFO("IndexCount: {:d}", pBuffer->GetCount());
			// size?? format?
			
		}

		// write meshes
		DV_CORE_INFO("Mesh Count: {:d}", GetMeshCount());
		for (size_t i = 0; i < m_Meshes.size(); ++i)
		{
		}
		
		// write morphs
		// write skeleton
		// write bounding box
		// write mesh centers
		// write meta data
	}

	bool DvModel::SetMesh(unsigned int index, DvMesh* pMesh)
	{
		if (index > m_Meshes.size())
		{
			DV_CORE_ERROR("메시 인덱스 범위를 벗어났습니다.");
			return false;
		}

		m_Meshes[index] = pMesh;

		return true;
	}

	void DvModel::SetMeshCount(unsigned int count)
	{
		m_Meshes.resize(count);
		// bone mappings
		// centers 역시 리사이즈
	}

	bool DvModel::SetIndexBuffers(const std::vector<DvIndexBuffer*>& buffers)
	{
		for (const auto pBuffer : buffers)
		{
			if (!pBuffer)
			{
				DV_CORE_ERROR("모델의 인덱스 버퍼가 비어있습니다.");
				return false;
			}
		}

		m_IndexBuffers = buffers;
		
		return true;
	}
}