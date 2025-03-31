#pragma once

namespace Dive
{
	// ���� ModelFactory�� Mesh���� UINT32�� �����ϱ� ������
	// ���Ҹ� void*�� �ٲ� ���� ����.
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

		static std::shared_ptr<IndexBuffer> Generate(const void* data, UINT32 count);

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		DXGI_FORMAT m_Format = DXGI_FORMAT_UNKNOWN;
		UINT32 m_Count = 0;
	};
}