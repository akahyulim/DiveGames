#pragma once

namespace Dive
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();

		template<typename T>
		bool Create(const std::vector<T>& indices)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_Count = static_cast<uint32_t>(indices.size());

			return createBuffer(static_cast<const void*>(indices.data()));
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
		DXGI_FORMAT GetFormat() const { return m_Stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; }

	private:
		bool createBuffer(const void* pIndices);

	private:
		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride;
		uint32_t m_Count;
	};
}