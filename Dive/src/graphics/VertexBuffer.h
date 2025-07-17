#pragma once

namespace Dive
{
	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer() = default;

		bool Create(const void* data, uint32_t stride, uint32_t count);

		ID3D11Buffer* GetBuffer() const { return m_Buffer.Get(); }
		uint32_t GetStride() const { return m_Stride; }
		uint32_t GetCount() const { return m_Count; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
		uint32_t m_Stride = 0;
		uint32_t m_Count = 0;
	};
}
