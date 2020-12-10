#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;

	class VertexBuffer : public Object
	{
		DIVE_OBJECT(VertexBuffer, Object);

	public:
		VertexBuffer(Context* context);
		~VertexBuffer();

		template<class T>
		bool Create(const std::vector<T>& vertices)
		{
			m_bDynamic		= false;
			m_stride		= static_cast<unsigned int>(sizeof(T));
			m_vertexCount	= static_cast<unsigned int>(vertices.size());

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		template<class T>
		bool Create(const T* vertices, const unsigned int vertexCount)
		{
			m_bDynamic		= false;
			m_stride		= static_cast<unsigned int>(sizeof(T));
			m_vertexCount	= vertexCount;

			return createBuffer(static_cast<const void*>(vertices));
		}

		template<class T>
		bool CreateDynamic(const unsigned int vertexCount)
		{
			m_bDynamic		= true;
			m_stride		= static_cast<unsigned int>(sizeof(T));
			m_vertexCount	= vertexCount;

			return createBuffer(nullptr);
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer()		const { return m_buffer; }
		unsigned int GetStride()		const { return m_stride; }
		unsigned int GetVertexCount()	const { return m_vertexCount; }

	private:
		bool createBuffer(const void* vertices);

	private:
		ID3D11Buffer* m_buffer = nullptr;

		bool m_bDynamic = false;
		
		unsigned int m_stride		= 0;
		unsigned int m_vertexCount	= 0;
	};
}

