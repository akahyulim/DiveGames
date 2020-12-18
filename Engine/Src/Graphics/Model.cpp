#include "DivePch.h"
#include "Model.h"
#include "Mesh.h"
#include "Core/Context.h"
#include "Core/Log.h"
#include "D3D11/VertexTypes.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"


namespace Dive
{
	Model::Model(Context * context)
		: Resource(context, eResourceType::Model)
	{
		m_mesh = std::make_unique<Mesh>();
	}

	Model::~Model()
	{
	}

	void Model::SetGeometry(std::vector<Vertex_PosTexNorTan>& vertices, std::vector<unsigned int>& indices)
	{
		if (vertices.empty() || indices.empty())
		{
			CORE_ERROR("");
			return;
		}

		m_mesh->SetVertices(vertices);
		m_mesh->SetIndices(indices);
	}

	bool Model::createGeometryBuffers()
	{
		const auto& vertices = m_mesh->GetVertices();
		if (!vertices.empty())
		{
			m_vertexBuffer = std::make_shared<VertexBuffer>(m_context);
			if (!m_vertexBuffer->Create(vertices))
			{
				CORE_ERROR("");
				return false;
			}
		}
		else
		{
			CORE_ERROR("");
			return false;
		}

		const auto& indices = m_mesh->GetIndices();
		if (!indices.empty())
		{
			m_indexBuffer = std::make_shared<IndexBuffer>(m_context);
			if (!m_indexBuffer->Create(indices))
			{
				CORE_ERROR("");
				return false;
			}
		}
		else
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
}