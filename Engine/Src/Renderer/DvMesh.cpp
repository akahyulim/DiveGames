#include "divepch.h"
#include "DvMesh.h"
#include "Base/Base.h"
#include "Graphics/DvVertexBuffer.h"
#include "Graphics/DvIndexBuffer.h"

namespace Dive
{
	DvMesh::DvMesh()
	{
		SetNumVertexBuffers(1);
	}

	DvVertexBuffer* DvMesh::GetVertexBuffer(unsigned int index) const
	{
		return index < static_cast<unsigned int>(m_VertexBuffers.size()) ? m_VertexBuffers[index] : nullptr;
	}

	void DvMesh::SetVertexBuffer(unsigned int index, DvVertexBuffer* pBuffer)
	{
		if (index >= static_cast<unsigned int>(m_VertexBuffers.size()))
		{
			DV_CORE_ERROR("�߸��� �ε����� ���޹޾ҽ��ϴ�.");
			return;
		}

		m_VertexBuffers[index] = pBuffer;
	}

	void DvMesh::SetNumVertexBuffers(unsigned int num)
	{
		m_VertexBuffers.resize(num);
	}
}