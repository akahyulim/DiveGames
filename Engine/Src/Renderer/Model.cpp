#include "divepch.h"
#include "Model.h"
#include "Mesh.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "IO/FileStream.h"
#include "IO/Log.h"

// 1. Vertex, Index Buffer를 왜 직접 가지고 있을까?
// 2. 계층구조는 어떻게 관리할까?

namespace Dive
{
	Model::Model(Context* pContext)
		: Resource(pContext)
	{
	}

	Model::~Model()
	{
		for (auto pMesh : m_Meshes)
			DV_DELETE(pMesh);
		m_Meshes.clear();

		DV_LOG_ENGINE_TRACE("Model 소멸 완료({:s})", GetName());
	}
	
	bool Model::Load(FileStream* pDeserializer)
	{
		if (!pDeserializer)
			return false;

		// fileID 확인.
		auto fileID = pDeserializer->ReadAs<std::string>();
		if ("MODEL" != fileID)
		{
			return false;
		}
		DV_LOG_ENGINE_DEBUG("fileID: {:s}", fileID);

		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
		m_Meshes.clear();

		// 정점	버퍼 개수 설정.
		unsigned int numVertexBuffers = 0;
		pDeserializer->Read(&numVertexBuffers);
		DV_LOG_ENGINE_DEBUG("numVertexBuffers: {:d}", numVertexBuffers);
		m_VertexBuffers.resize(numVertexBuffers);
		
		for (unsigned int i = 0; i < numVertexBuffers; ++i)
		{
			auto pVertexBuffer = new VertexBuffer(m_pContext);

			// 정점 개수 설정.
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

			// 아래 구문은 에바다.
			// 여기에선 버퍼를 구성해야만 한다.

			// 정점 개수, 구성 요소 설정.
			pVertexBuffer->SetSize(numVertices, elements);

			// 정점 데이터 설정.
			void* pDest = pVertexBuffer->Map();
			pDeserializer->Read(pDest, pVertexBuffer->GetCount() * pVertexBuffer->GetSize());
			pVertexBuffer->Unmap();

			m_VertexBuffers[i] = pVertexBuffer;
		}

		// 인덱스 버퍼 개수 설정.
		// => 버퍼 클래스를 수정 중이라 추후 변경이 필요하다
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

			// 인덱스 개수, 크기 설정.
			//pIndexBuffer->SetStride(numIndices, indexSize > sizeof(unsigned short));

			// 인덱스 데이터 설정.
			void* pDest = pIndexBuffer->Map();
			pDeserializer->Read(pDest, pIndexBuffer->GetCount() * pIndexBuffer->GetStride());
			pIndexBuffer->Unmap();

			m_IndexBuffers[i] = pIndexBuffer;
		}

		// 메시 개수 기록
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

	// 그러고보니 계층구조 정보가 전혀 없다.
	// Mesh가 stride 정보도 가질텐데... 아직 모르겠다.
	bool Model::Save(FileStream* pSerializer)
	{
		if (!pSerializer)
			return false;

		// fileID 기록.
		pSerializer->Write("MODEL");

		// 정점	버퍼 개수 기록.
		pSerializer->Write(static_cast<unsigned int>(m_VertexBuffers.size()));
		for (unsigned int i = 0; i < (unsigned int)m_VertexBuffers.size(); ++i)
		{
			auto* pVertexBuffer = m_VertexBuffers[i];

			// 정점 개수 기록.
			pSerializer->Write(static_cast<unsigned int>(pVertexBuffer->GetCount()));

			// 요소 개수 기록.
			auto elements = pVertexBuffer->GetElements();
			pSerializer->Write(static_cast<unsigned int>(elements.size()));

			// 요소의 타입, 시맨틱, 인덱스 기록.
			for (unsigned int j = 0; j < elements.size(); ++j)
			{
				unsigned int elementsDesc =
					(static_cast<unsigned int>(elements[j].m_Type)) |
					(static_cast<unsigned int>(elements[j].m_Semantic << 8u)) |
					(static_cast<unsigned int>(elements[j].m_Index << 16u));
					
				pSerializer->Write(elementsDesc);
			}

			// 정점 데이터 기록.
			pSerializer->Write(m_VertexBuffers[i]->GetData(), m_VertexBuffers[i]->GetCount() * m_VertexBuffers[i]->GetSize());
		}

		// 인덱스 버퍼 개수 기록.
		pSerializer->Write(static_cast<unsigned int>(m_IndexBuffers.size()));
		for (unsigned int i = 0; i < (unsigned int)m_IndexBuffers.size(); ++i)
		{
			auto* pIndexBuffer = m_IndexBuffers[i];

			// 인덱스 개수 기록.
			pSerializer->Write(static_cast<unsigned int>(pIndexBuffer->GetCount()));

			// 인덱스 크기 기록.
			pSerializer->Write(static_cast<unsigned int>(pIndexBuffer->GetStride()));
			
			// 인덱스 데이터 기록.
			//pSerializer->Write(pIndexBuffer->GetData(), pIndexBuffer->GetIndexCount() * pIndexBuffer->GetIndexSize());
		}

		// 메시 개수 기록
		pSerializer->Write(static_cast<unsigned int>(m_Meshes.size()));
		for (unsigned int i = 0; i < (unsigned int)m_Meshes.size(); ++i)
		{

		}

		// write mesh centers

		return true;
	}

	void Model::RegisterObject(Context* pContext)
	{
		pContext->RegisterFactory<Model>();
	}

	bool Model::SetVertexBuffers(const std::vector<VertexBuffer*>& pBuffers)
	{
		for (unsigned int i = 0; i < (unsigned int)pBuffers.size(); ++i)
		{
			if (!pBuffers[i])
			{
				DV_LOG_ENGINE_ERROR("Model::SetVertexBuffers - 버퍼를 잘못 전달 받았습니다.");
				return false;
			}

			// shadow data도 있어야 한다.
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
				DV_LOG_ENGINE_ERROR("Model::SetIndexBuffers - 버퍼를 잘못 전달 받았습니다.");
				return false;
			}

			// shadow data도 있어야 한다.
		}

		m_IndexBuffers = pBuffers;

		return true;
	}
	
	void Model::SetNumMeshes(size_t num)
	{
		m_Meshes.resize(num);
	}
	
	Mesh* Model::GetMesh(size_t index)
	{
		if (index >= m_Meshes.size())
			return nullptr;

		return m_Meshes[index];
	}
	
	bool Model::SetMesh(size_t index, Mesh* pMesh)
	{
		if (index >= m_Meshes.size())
		{
			DV_LOG_ENGINE_ERROR("Model::SetMesh - 메시 인덱스를 잘못 전달받았습니다.");
			return false;
		}

		m_Meshes[index] = pMesh;

		return true;
	}
}