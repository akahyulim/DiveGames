#pragma once
#include "Resource/Resource.h"
#include "D3D11/VertexTypes.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"


namespace Dive
{
	class Context;
	class Mesh;

	class MeshFilter : public Resource
	{
		DIVE_OBJECT(MeshFilter, Resource);

	public:
		MeshFilter(Context* context);
		~MeshFilter();

		void Clear();

		bool CreateMesh(std::string name, const std::vector<Vertex_PosTexNorTan>& vertices, const std::vector<unsigned int>& indices);
		bool SetMesh(std::string name, Mesh* mesh);
		Mesh* GetMesh() const { return m_mesh; }

		VertexBuffer* GetVertexBuffer()	const { return m_vertexBuffer; }
		IndexBuffer* GetIndexBuffer()	const { return m_indexBuffer; }

		unsigned int GetVertexCount()	const { return m_vertexCount; }
		unsigned int GetIndexCount()	const { return m_indexCount; }

	private:
		MeshFilter(const MeshFilter&)				= delete;
		MeshFilter& operator=(const MeshFilter&)	= delete;

		bool createBuffer();

	private:
		Mesh* m_mesh;
		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
		unsigned int m_vertexCount;
		unsigned int m_indexCount;
	};
}