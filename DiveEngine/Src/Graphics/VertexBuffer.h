#pragma once

namespace Dive
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		template<typename T>
		bool Create(const std::vector<T>& vertices)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_Count = static_cast<uint32_t>(vertices.size());

			return createBuffer(static_cast<const void*>(vertices.data()));
		}

		template<typename T>
		bool Create(const T* pVertices, uint32_t count)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_Count = count;

			return createBuffer(static_cast<const void*>(pVertices));
		}

		template<typename T>
		bool CreateDynamic(uint32_t count)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_Count = count;

			return createBuffer(nullptr);
		}

		void Destroy();

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		uint32_t GetStride() const { return m_Stride; }
		uint32_t GetCount() const { return m_Count; }

	private:
		bool createBuffer(const void* pVertices);

	private:
		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride;
		uint32_t m_Count;
	};
}