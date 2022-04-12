#pragma once

namespace Dive
{
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer();

		bool Create(const std::vector<unsigned int>& indices)
		{
			m_Stride	= static_cast<unsigned int>(sizeof(unsigned int));
			m_Count		= static_cast<unsigned int>(indices.size());

			return createBuffer(static_cast<const void*>(indices.data()));
		}

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_pBuffer; }
		DXGI_FORMAT GetFormat() const { return DXGI_FORMAT_R32_UINT; }
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