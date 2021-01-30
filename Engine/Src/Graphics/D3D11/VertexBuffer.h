#pragma once
#include "Core/Object.h"
#include "../VertexTypes.h"

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
			m_vertexType	= VertexTypeToEnum<T>();

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		template<class T>
		bool Create(const T* vertices, const unsigned int vertexCount)
		{
			m_bDynamic		= false;
			m_stride		= static_cast<unsigned int>(sizeof(T));
			m_vertexCount	= vertexCount;
			m_vertexType	= VertexTypeToEnum<T>();

			return createBuffer(static_cast<const void*>(vertices));
		}

		template<class T>
		bool CreateDynamic(const unsigned int vertexCount)
		{
			m_bDynamic		= true;
			m_stride		= static_cast<unsigned int>(sizeof(T));
			m_vertexCount	= vertexCount;
			m_vertexType	= VertexTypeToEnum<T>();

			return createBuffer(nullptr);
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer()			const { return m_buffer; }
		unsigned int GetStride()			const { return m_stride; }
		unsigned int GetVertexCount()		const { return m_vertexCount; }
		eVertexType GetVertexType()			const { return m_vertexType; }

	private:
		bool createBuffer(const void* vertices);

	private:
		ID3D11Buffer* m_buffer;

		bool m_bDynamic;
		unsigned int m_stride;
		unsigned int m_vertexCount;
		eVertexType m_vertexType;
	};
}

