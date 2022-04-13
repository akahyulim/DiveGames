#pragma once

namespace Dive
{
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer() { Destroy(); }

		template<typename T>
		bool Create(const std::vector<T>& indices)
		{
			m_Stride	= static_cast<unsigned int>(sizeof(T));
			m_Count		= static_cast<unsigned int>(indices.size());
			m_bDynamic	= false;

			return createBuffer(static_cast<const void*>(indices.data()));
		}
		template<typename T>
		bool CreateDynamic(unsigned int indexCount)
		{
			m_Stride	= static_cast<unsigned int>(sizeof(T));
			m_Count		= indexCount;
			m_bDynamic	= true;

			return createBuffer(nullptr);
		}
		void Destroy();

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		DXGI_FORMAT GetFormat() const { return m_Stride == sizeof(unsigned int) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT; }
		unsigned int GetStride() const { return m_Stride; }
		unsigned int GetCount() const { return m_Count; }
		bool IsDynamic() const { return m_bDynamic; }

	private:
		bool createBuffer(const void* pData);

	private:
		ID3D11Buffer* m_pBuffer		= nullptr;
		unsigned int m_Stride		= 0;
		unsigned int m_Count		= 0;
		bool m_bDynamic				= false;
	};
}