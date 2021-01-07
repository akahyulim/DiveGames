#include "DivePch.h"
#include "MeshFilter.h"
#include "Mesh.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	MeshFilter::MeshFilter(Context * context)
		: Resource(context, eResourceType::MeshFilter),
		m_mesh(nullptr),
		m_vertexBuffer(nullptr),
		m_indexBuffer(nullptr),
		m_vertexCount(0),
		m_indexCount(0)
	{
	}

	MeshFilter::~MeshFilter()
	{
		Clear();
	}

	void MeshFilter::Clear()
	{
		SAFE_DELETE(m_indexBuffer);
		SAFE_DELETE(m_vertexBuffer);
		SAFE_DELETE(m_mesh);
	}

	bool MeshFilter::CreateMesh(std::string name, const std::vector<Vertex_PosTexNorTan>& vertices, const std::vector<unsigned int>& indices)
	{
		if (vertices.empty() || indices.empty())
			return false;

		if (m_mesh)
			Clear();

		m_mesh = new Mesh;
		m_mesh->SetVertices(vertices);
		m_mesh->SetIndices(indices);

		m_name = std::move(name);

		return createBuffer();
	}

	bool MeshFilter::SetMesh(std::string name, Mesh * mesh)
	{
		if (!mesh)
			return false;

		if (m_mesh)
			Clear();

		m_mesh = std::move(mesh);	// 좀 더 알아보자.
		m_name = std::move(name);

		return createBuffer();
	}

	bool MeshFilter::createBuffer()
	{
		if (!m_mesh)
			return false;

		{
			m_vertexCount	= m_mesh->GetVerticesCount();
			m_indexCount	= m_mesh->GetIndicesCount();
		}

		{
			m_vertexBuffer = new VertexBuffer(m_context);
			if (!m_vertexBuffer->Create(m_mesh->GetVertices()))
			{
				CORE_ERROR("");
				return false;
			}
		}

		{
			m_indexBuffer = new IndexBuffer(m_context);
			if (!m_indexBuffer->Create(m_mesh->GetIndices()))
			{
				CORE_ERROR("");
				return false;
			}
		}

		return true;
	}
}