#include "divepch.h"
#include "Model.h"
#include "Mesh.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Core/Context.h"
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
		return false;
	}

	bool Model::Save(FileStream* pSerializer)
	{
		// Vertex Buffer
		{
			auto vertexCount = m_VertexBuffers.size();
			DV_LOG_ENGINE_DEBUG("VertexBuffer Count: {:d}", vertexCount);

			for (const auto* pBuffer : m_VertexBuffers)
			{
				auto numVertices = pBuffer->GetVertexCount();
				DV_LOG_ENGINE_DEBUG("Num Vertices: {:d}", numVertices);

				auto numElements = pBuffer->GetElements().size();
				DV_LOG_ENGINE_DEBUG("Num Vertex Elements: {:d}", numElements);

				for (const auto element : pBuffer->GetElements())
				{
					// element�� Ÿ��, �ø�ƽ, �ε���

				}

				// ���� ������ -> �״�� ����ȭ
			}
		}

		// Index Buffer
		{
			auto indexCount = m_IndexBuffers.size();
			DV_LOG_ENGINE_DEBUG("IndexBuffer Count: {:d}", indexCount);

			for (const auto* pBuffer : m_IndexBuffers)
			{
				auto numIndex = pBuffer->GetIndexCount();
				DV_LOG_ENGINE_DEBUG("Num Indices: {:d}", numIndex);

				auto indexSize = pBuffer->GetIndexSize();
				DV_LOG_ENGINE_DEBUG("Index Size: {:d}", indexSize);

				// �ε��� ������ -> �״�� ����ȭ 
			}
		}

		return false;
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