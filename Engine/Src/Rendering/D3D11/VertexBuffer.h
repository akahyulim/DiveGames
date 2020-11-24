#pragma once
#include "Core/Object.h"

namespace Dive
{
	class RenderDevice;

	class VertexBuffer : public Object
	{
	public:
		VertexBuffer(const std::shared_ptr<RenderDevice>& device);
		~VertexBuffer();

		template<class T>
		bool Create(const std::vector<T>& vertices)
		{
			m_bDynamic		= false;
			m_Stride		= static_cast<unsigned int>(sizeof(T));
			m_VertexCount	= static_cast<unsigned int>(vertices.size());

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		template<class T>
		bool Create(const T* vertices, const unsigned int vertexCount)
		{
			m_bDynamic		= false;
			m_Stride		= static_cast<unsigned int>(sizeof(T));
			m_VertexCount	= vertexCount;

			return createBuffer(static_cast<const void*>(vertices));
		}

		template<class T>
		bool CreateDynamic(const unsigned int vertexCount)
		{
			m_bDynamic		= true;
			m_Stride		= static_cast<unsigned int>(sizeof(T));
			m_VertexCount	= vertexCount;

			return createBuffer(nullptr);
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() const		{ return m_Buffer; }
		unsigned int GetStride() const		{ return m_Stride; }
		unsigned int GetVertexCount() const { return m_VertexCount; }

	private:
		bool createBuffer(const void* vertices);

	private:
		std::shared_ptr<RenderDevice> m_Device;
		ID3D11Buffer* m_Buffer = nullptr;

		bool m_bDynamic = false;
		
		unsigned int m_Stride		= 0;
		unsigned int m_VertexCount	= 0;
	};
}

