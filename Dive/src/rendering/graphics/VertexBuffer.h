#pragma once

namespace Dive
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		bool CreateBuffer(const void* pData, uint32_t stride, uint32_t count);
		void Destroy();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		uint32_t GetStride() const { return m_Stride; }
		uint32_t GetCount() const { return m_Count; }

	public:
		static VertexBuffer* Create(const void* pData, uint32_t stride, uint32_t count);

	private:
		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride;
		uint32_t m_Count;
	};
}