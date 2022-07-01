#include "divepch.h"
#include "DvModel.h"
#include "DvMesh.h"	
#include "Base/Base.h"
#include "Graphics/VertexBuffer.h"
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
			DV_CORE_INFO("Vertex Count: {:d}", pBuffer->GetCount());
			// element size
			// element desc
			// start
			// count: ���� count�ʹ� �ٸ� ��...
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

	bool DvModel::SetIndexBuffers(const std::vector<DvIndexBuffer*>& buffers)
	{
		for (const auto pBuffer : buffers)
		{
			if (!pBuffer)
			{
				DV_CORE_ERROR("���� �ε��� ���۰� ����ֽ��ϴ�.");
				return false;
			}
		}

		m_IndexBuffers = buffers;
		
		return true;
	}
}