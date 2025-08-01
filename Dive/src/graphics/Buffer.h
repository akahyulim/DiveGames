#pragma once

namespace Dive
{
	enum class eBufferType
	{
		VertexBuffer,
		IndexBuffer
	};

	class Buffer
	{
	public:
		Buffer(ID3D11Device* device, eBufferType type, const void* rawData, uint32_t stride, uint32_t count);
		~Buffer();

		void Bind(ID3D11DeviceContext* deviceContext) const;

		ID3D11Buffer* GetBuffer() const { return m_buffer.Get(); }
		eBufferType GetType() const { return m_type; }
		uint32_t GetStride() const { return m_stride; }
		uint32_t GetCount() const { return m_count; }

		explicit operator bool() const { return m_buffer != nullptr; }

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

		eBufferType m_type = eBufferType::VertexBuffer;

		uint32_t m_stride;
		uint32_t m_count;
	};
}