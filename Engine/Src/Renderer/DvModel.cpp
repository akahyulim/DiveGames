#include "divepch.h"
#include "DvModel.h"
#include "DvMesh.h"	
#include "Base/Base.h"
#include "Graphics/DvVertexBuffer.h"
#include "Graphics/DvIndexBuffer.h"

namespace Dive
{
	// ���� ������ ���� �� ����
	bool DvModel::LoadFromFile(const std::string& filepath)
	{
		return false;
	}

	bool DvModel::SaveToFile(const std::string& filepath)
	{
		return false;
	}

	// ����ȭ �׽�Ʈ
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
			// count: ���� count�ʹ� �ٸ� ��...
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
			// data�� count * size
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
			DV_CORE_ERROR("�޽� �ε��� ������ ������ϴ�.");
			return false;
		}

		m_Meshes[index] = pMesh;

		return true;
	}

	void DvModel::SetMeshCount(unsigned int count)
	{
		m_Meshes.resize(count);
		// bone mappings
		// centers ���� ��������
	}

	// �Ű� ������ �� �߰��ؾ� �Ѵ�.
	bool DvModel::SetVertexBuffers(const std::vector<DvVertexBuffer*> buffers)
	{
		for (const auto pBuffer : buffers)
		{
			if (!pBuffer)
			{
				DV_CORE_ERROR("���� ���� ���۰� ����ֽ��ϴ�.");
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
				DV_CORE_ERROR("���� �ε��� ���۰� ����ֽ��ϴ�.");
				return false;
			}

			// shadowed�� Ȯ���ؾ� �Ѵ�?
		}

		m_IndexBuffers = buffers;
		
		return true;
	}
}