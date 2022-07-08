#include "divepch.h"
#include "DvModel.h"
#include "DvMesh.h"	
#include "Base/Base.h"
#include "Graphics/DvVertexBuffer.h"
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
			DV_CORE_INFO("Vertex Count: {:d}", pBuffer->GetVertexCount());
			// element size
			DV_CORE_INFO("Vertex Size: {:d}", pBuffer->GetVertexSize());
			// element desc
			// start
			// count: 위의 count와는 다른 듯...
		}

		// write index buffers
		// buffers size
		DV_CORE_INFO("IndexBuffer Count: {:d}", m_IndexBuffers.size());
		for (auto pIndexBuffer : m_IndexBuffers)
		{
			// count
			DV_CORE_INFO("Index Count: {:d}", pIndexBuffer->GetIndexCount());
			// size
			DV_CORE_INFO("Index Size: {:d}", pIndexBuffer->GetIndexSize());
			// data를 count * size
			auto pData = (unsigned short*)pIndexBuffer->GetData();
			for (unsigned int i = 0; i < pIndexBuffer->GetIndexCount();)
			{
			//	DV_CORE_INFO("Index {0:d}, {1:d}, {2:d}", pData[i], pData[i + 1], pData[i + 2]);
				i += 3;
			}

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

	// 매개 변수를 더 추가해야 한다.
	bool DvModel::SetVertexBuffers(const std::vector<DvVertexBuffer*> buffers)
	{
		for (const auto pBuffer : buffers)
		{
			if (!pBuffer)
			{
				DV_CORE_ERROR("모델의 정점 버퍼가 비어있습니다.");
				return false;
			}
		}

		m_VertexBuffers = buffers;

		return true;
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

			// shadowed도 확인해야 한다?
		}

		m_IndexBuffers = buffers;
		
		return true;
	}
}