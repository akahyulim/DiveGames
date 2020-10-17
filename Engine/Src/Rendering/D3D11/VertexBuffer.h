#pragma once
#include "Core/Object.h"

namespace Dive
{
	class RenderDevice;

	class VertexBuffer : public Object
	{
	public:
		VertexBuffer(RenderDevice* device);
		~VertexBuffer();

		template<class T>
		bool Create(const std::vector<T>& vertices)
		{
			m_is_dynamic = false;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_vertex_count = static_cast<unsigned int>(vertices.size());

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		template<class T>
		bool Create(const T* pVertices, const unsigned int vertexCount)
		{
			m_is_dynamic = false;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_vertex_count = vertexCount;

			return createBuffer(static_cast<const void*>(pVertices));
		}

		template<class T>
		bool CreateDynamic(const unsigned int vertexCount)
		{
			m_is_dynamic = true;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_vertex_count = vertexCount;

			return createBuffer(nullptr);
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const { return m_buffer.get(); }
		unsigned int GetStride() const { return m_stride; }
		unsigned int GetVertexCount() const { return m_vertex_count; }

	private:
		bool createBuffer(const void* pVertices);

	private:
		RenderDevice* m_device;
		std::shared_ptr<ID3D11Buffer> m_buffer;

		bool m_is_dynamic = false;
		
		unsigned int m_stride = 0;
		unsigned int m_vertex_count = 0;
	};
}

