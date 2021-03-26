#pragma once
#include "GraphicsDevice.h"
#include <vector>

namespace Dive
{
	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer() = default;

		template<typename T>
		bool Create(const std::vector<T>& vertices)
		{
			m_bDynamic = false;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_vertexCount = static_cast<unsigned int>(vertices.size());

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		template<typename T>
		bool Create(const T* vertices, const unsigned int vertexCount)
		{
			m_bDynamic = false;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_vertexCount = vertexCount;
			//m_vertexType = VertexTypeToEnum<T>();

			return createBuffer(static_cast<const void*>(vertices));
		}

		template<typename T>
		bool CreateDynamic(const unsigned int vertexCount)
		{
			m_bDynamic = true;
			m_stride = static_cast<unsigned int>(sizeof(T));
			m_vertexCount = vertexCount;
			//m_vertexType = VertexTypeToEnum<T>();

			return createBuffer(nullptr);
		}

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_buffer.Get(); }
		bool IsDynamicBuffer() const { return m_bDynamic; }
		unsigned int GetStride() const { return m_stride; }
		unsigned int GetCount() const { return m_vertexCount; }

	private:
		bool createBuffer(const void* datas);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
		bool m_bDynamic = false;
		unsigned int m_stride = 0;
		unsigned int m_vertexCount = 0;
		// type???
	};
}