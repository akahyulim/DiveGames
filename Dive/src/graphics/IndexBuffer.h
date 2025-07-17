#pragma once

namespace Dive
{
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer() = default;

		bool Create(const void* data, uint32_t indexCount, uint32_t vertexCount);

		ID3D11Buffer* GetBuffer() const { return m_Buffer.Get(); }

		void SetFormat(DXGI_FORMAT format) { m_Format = format; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		
		uint32_t GetCount() const { return m_Count; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		uint32_t m_Count = 0;
	};
}
