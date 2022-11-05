#include "divepch.h"
#include "Model.h"
#include "Mesh.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/Context.h"
#include "IO/FileStream.h"
#include "IO/Log.h"

namespace Dive
{
	Model::Model(Context* pContext)
		: Resource(pContext)
	{
	}

	Model::~Model()
	{
	}

	void Model::RegisterObject(Context* pContext)
	{
		pContext->RegisterFactory<Model>();
	}

	bool Model::Load(FileStream* pDeserializer)
	{
		if (!pDeserializer)
			return false;

		// fileID Ȯ��.
		auto fileID = pDeserializer->ReadAs<std::string>();
		if ("MODEL" != fileID)
		{
			return false;
		}
		DV_LOG_ENGINE_DEBUG("fileID: {:s}", fileID);

		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
		m_Meshes.clear();

		// ����	���� ���� ����.
		unsigned int numVertexBuffers = 0;
		pDeserializer->Read(&numVertexBuffers);
		DV_LOG_ENGINE_DEBUG("numVertexBuffers: {:d}", numVertexBuffers);
		m_VertexBuffers.resize(numVertexBuffers);
		
		for (unsigned int i = 0; i < numVertexBuffers; ++i)
		{
			auto pVertexBuffer = new VertexBuffer(m_pContext);

			// ���� ���� ����.
			unsigned int numVertices = 0;
			pDeserializer->Read(&numVertices);
			DV_LOG_ENGINE_DEBUG("numVertices: {:d}", numVertices);

			unsigned int numElements = 0;
			pDeserializer->Read(&numElements);
			DV_LOG_ENGINE_DEBUG("numElements: {:d}", numElements);			
			std::vector<VertexElement> elements;
			for (unsigned int j = 0; j < numElements; ++j)
			{
				unsigned int elementDesc = 0;
				pDeserializer->Read(&elementDesc);

				auto type = static_cast<eVertexElementType>(elementDesc & 0xffu);
				auto semantic = static_cast<eVertexElementSemantic>((elementDesc >> 8u) & 0xffu);
				auto index = static_cast<unsigned char>((elementDesc >> 16u) & 0xffu);

				elements.emplace_back(type, semantic, index);
			}

			// �Ʒ� ������ ���ٴ�.
			// ���⿡�� ���۸� �����ؾ߸� �Ѵ�.

			// ���� ����, ���� ��� ����.
			pVertexBuffer->SetSize(numVertices, elements);

			// ���� ������ ����.
			void* pDest = pVertexBuffer->Map();
			pDeserializer->Read(pDest, pVertexBuffer->GetVertexCount() * pVertexBuffer->GetVertexSize());
			pVertexBuffer->Unmap();

			m_VertexBuffers[i] = pVertexBuffer;
		}

		// �ε��� ���� ���� ����.
		unsigned int numIndexBuffers = 0;
		pDeserializer->Read(&numIndexBuffers);
		DV_LOG_ENGINE_DEBUG("numIndexBuffers: {:d}", numIndexBuffers);
		m_IndexBuffers.resize(numIndexBuffers);
		for (unsigned int i = 0; i < numIndexBuffers; ++i)
		{
			auto* pIndexBuffer = new IndexBuffer(m_pContext);

			unsigned int numIndices = 0;
			pDeserializer->Read(&numIndices);
			DV_LOG_ENGINE_DEBUG("numIndices: {:d}", numIndices);

			unsigned int indexSize = 0;
			pDeserializer->Read(&indexSize);
			DV_LOG_ENGINE_DEBUG("indexSize: {:d}", indexSize);

			// �ε��� ����, ũ�� ����.
			pIndexBuffer->SetSize(numIndices, indexSize > sizeof(unsigned short));

			// �ε��� ������ ����.
			void* pDest = pIndexBuffer->Map();
			pDeserializer->Read(pDest, pIndexBuffer->GetIndexCount() * pIndexBuffer->GetIndexSize());
			pIndexBuffer->Unmap();

			m_IndexBuffers[i] = pIndexBuffer;
		}

		// �޽� ���� ���
		unsigned int numMeshes = 0;
		pDeserializer->Read(&numMeshes);
		DV_LOG_ENGINE_DEBUG("numMeshes: {:d}", numMeshes);
		m_Meshes.resize(numMeshes);
		for (unsigned int i = 0; i < numMeshes; ++i)
		{

		}

		// write mesh centers

		return true;
	}

	// �׷����� �������� ������ ���� ����.
	// Mesh�� stride ������ �����ٵ�... ���� �𸣰ڴ�.
	bool Model::Save(FileStream* pSerializer)
	{
		if (!pSerializer)
			return false;

		// fileID ���.
		pSerializer->Write("MODEL");

		// ����	���� ���� ���.
		pSerializer->Write(static_cast<unsigned int>(m_VertexBuffers.size()));
		for (unsigned int i = 0; i < (unsigned int)m_VertexBuffers.size(); ++i)
		{
			auto* pVertexBuffer = m_VertexBuffers[i];

			// ���� ���� ���.
			pSerializer->Write(static_cast<unsigned int>(pVertexBuffer->GetVertexCount()));

			// ��� ���� ���.
			auto elements = pVertexBuffer->GetElements();
			pSerializer->Write(static_cast<unsigned int>(elements.size()));

			// ����� Ÿ��, �ø�ƽ, �ε��� ���.
			for (unsigned int j = 0; j < elements.size(); ++j)
			{
				unsigned int elementsDesc =
					(static_cast<unsigned int>(elements[j].m_Type)) |
					(static_cast<unsigned int>(elements[j].m_Semantic << 8u)) |
					(static_cast<unsigned int>(elements[j].m_Index << 16u));
					
				pSerializer->Write(elementsDesc);
			}

			// ���� ������ ���.
			pSerializer->Write(m_VertexBuffers[i]->GetData(), m_VertexBuffers[i]->GetVertexCount() * m_VertexBuffers[i]->GetVertexSize());
		}

		// �ε��� ���� ���� ���.
		pSerializer->Write(static_cast<unsigned int>(m_IndexBuffers.size()));
		for (unsigned int i = 0; i < (unsigned int)m_IndexBuffers.size(); ++i)
		{
			auto* pIndexBuffer = m_IndexBuffers[i];

			// �ε��� ���� ���.
			pSerializer->Write(static_cast<unsigned int>(pIndexBuffer->GetIndexCount()));

			// �ε��� ũ�� ���.
			pSerializer->Write(static_cast<unsigned int>(pIndexBuffer->GetIndexSize()));
			
			// �ε��� ������ ���.
			pSerializer->Write(pIndexBuffer->GetData(), pIndexBuffer->GetIndexCount() * pIndexBuffer->GetIndexSize());
		}

		// �޽� ���� ���
		pSerializer->Write(static_cast<unsigned int>(m_Meshes.size()));
		for (unsigned int i = 0; i < (unsigned int)m_Meshes.size(); ++i)
		{

		}

		// write mesh centers

		return true;
	}

	bool Model::SetVertexBuffers(const std::vector<VertexBuffer*>& pBuffers)
	{
		for (unsigned int i = 0; i < (unsigned int)pBuffers.size(); ++i)
		{
			if (!pBuffers[i])
			{
				DV_LOG_ENGINE_ERROR("�� ��ü�� ���� ���۸� �߸� ���� �޾ҽ��ϴ�.");
				return false;
			}

			// shadow data�� �־�� �Ѵ�.
		}

		m_VertexBuffers = pBuffers;

		return true;
	}
	
	bool Model::SetIndexBuffers(const std::vector<IndexBuffer*>& pBuffers)
	{
		for (unsigned int i = 0; i < (unsigned int)pBuffers.size(); ++i)
		{
			if (!pBuffers[i])
			{
				DV_LOG_ENGINE_ERROR("�� ��ü�� �ε��� ���۸� �߸� ���� �޾ҽ��ϴ�.");
				return false;
			}

			// shadow data�� �־�� �Ѵ�.
		}

		m_IndexBuffers = pBuffers;

		return true;
	}
	
	void Model::SetNumMeshes(unsigned int num)
	{
		m_Meshes.resize((size_t)num);
	}
	
	Mesh* Model::GetMesh(unsigned int index)
	{
		if (index >= (unsigned int)m_Meshes.size())
			return nullptr;

		return m_Meshes[index];
	}
	
	bool Model::SetMesh(unsigned int index, Mesh* pMesh)
	{
		if (index >= (unsigned int)m_Meshes.size())
		{
			DV_LOG_ENGINE_ERROR("�޽� �ε����� �߸� ���޹޾ҽ��ϴ�.");
			return false;
		}

		m_Meshes[index] = pMesh;

		return true;
	}
}