#pragma once
#include "VertexTypes.h"
#include "Mesh.h"
#include "Resource/Resource.h"
#include "D3D11/VertexBuffer.h"
#include "D3D11/IndexBuffer.h"
#include "Core/Context.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


// Mesh 정보 저장 및 Buffer 생성
namespace Dive
{	
	class MeshFilter : public Resource
	{
		DIVE_OBJECT(MeshFilter, Resource);

	public:
		MeshFilter(Context* context);
		~MeshFilter();

		void Clear();

		template<typename VertexType, typename IndexType = unsigned int>
		bool CreateMesh(const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, std::string name = "");
		template<typename VertexType, typename IndexType = unsigned int>
		bool SetMesh(const Mesh<VertexType, typename IndexType>& mesh, std::string name = "");
		// 전달은 할 수 있지만 타입을 유추할 수 없다.
		void* GetMesh() const { return m_mesh; }

		VertexBuffer* GetVertexBuffer()	const { return m_vertexBuffer.get(); }
		unsigned int GetVertexCount()	const { return m_vertexCount; }
		eVertexType GetVertexType()		const { return m_vertexBuffer ? m_vertexBuffer->GetVertexType() : eVertexType::Unknown;}

		IndexBuffer* GetIndexBuffer()	const { return m_indexBuffer.get(); }
		unsigned int GetIndexCount()	const { return m_indexCount; }
		DXGI_FORMAT GetIndexFormat()	const { return m_indexBuffer ? m_indexBuffer->GetFormat() : DXGI_FORMAT_UNKNOWN; }

		unsigned int GetTriangleCount() const { return m_indexCount / 3; }

	private:
		MeshFilter(const MeshFilter&)				= delete;
		MeshFilter& operator=(const MeshFilter&)	= delete;

		template<typename VertexType, typename IndexType>
		bool createBuffer();

	private:
		void* m_mesh;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		unsigned int m_vertexCount;
		unsigned int m_indexCount;
	};

	template<typename VertexType, typename IndexType>
	inline bool MeshFilter::CreateMesh(const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, std::string name)
	{
		if (vertices.empty() || indices.empty())
		{
			CORE_ERROR("");
			return false;
		}

		m_name			= std::move(name);
		m_vertexCount	= static_cast<unsigned int>(vertices.size());
		m_indexCount	= static_cast<unsigned int>(indices.size());
		m_mesh			= (void*)new Mesh<T>();
		static_cast<Mesh<T>*>(m_mesh)->SetVertices(vertices);
		static_cast<Mesh<T>*>(m_mesh)->SetIndices(indices);
		
		return createBuffer<VertexType, IndexType>();
	}

	template<typename VertexType, typename IndexType>
	inline bool MeshFilter::SetMesh(const Mesh<VertexType, IndexType>& mesh, std::string name)
	{
		m_mesh			= (void*)new Mesh<VertexType, IndexType>(mesh);
		m_vertexCount	= mesh.GetVertexCount();
		m_indexCount	= mesh.GetIndexCount();
		m_name			= std::move(name);
		
		return createBuffer<VertexType, IndexType>();
	}

	template<typename VertexType, typename IndexType>
	inline bool MeshFilter::createBuffer()
	{
		if (!m_mesh)
		{
			CORE_ERROR("");
			return false;
		}

		// vertex buffer
		m_vertexBuffer = std::make_shared<VertexBuffer>(m_context);
		if (!m_vertexBuffer->Create<VertexType>(static_cast<Mesh<VertexType, IndexType>*>(m_mesh)->GetVertices()))
		{
			return false;
		}

		// index buffer
		m_indexBuffer = std::make_shared<IndexBuffer>(m_context);
		if (!m_indexBuffer->Create<IndexType>(static_cast<Mesh<VertexType, IndexType>*>(m_mesh)->GetIndices()))
		{
			return false;
		}

		return true;
	}
}