#pragma once

namespace Dive 
{
	class VertexBuffer 
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer();

		template<typename T>
		bool Create(const std::vector<T>& vertices)
		{
			m_Stride	= static_cast<unsigned int>(sizeof(T));
			m_Count		= static_cast<unsigned int>(vertices.size());

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		unsigned int GetStride() const { return m_Stride; }
		unsigned int GetCount() const { return m_Count; }

	private:
		bool createBuffer(const void* pData);

	private:
		ID3D11Buffer* m_pBuffer		= nullptr;
		
		unsigned int m_Stride		= 0;
		unsigned int m_Count		= 0;
	};
}