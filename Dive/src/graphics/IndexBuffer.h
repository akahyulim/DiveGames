#pragma once

namespace Dive
{
	// 현재 ModelFactory와 Mesh에서 UINT32로 구성하기 때문에
	// 섯불리 void*로 바꿀 수가 없다.
	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer();

		bool Create(const void* data, UINT32 count);
		void Release();

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		UINT32 GetCount() const { return m_Count; }

		static IndexBuffer* Generate(const void* data, UINT32 count);

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		UINT32 m_Count = 0;
	};
}
