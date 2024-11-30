#pragma once

namespace Dive
{
	// ���� ModelFactory�� Mesh���� uint32_t�� �����ϱ� ������
	// ���Ҹ� void*�� �ٲ� ���� ����.
	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(uint16_t* pIndices, uint32_t count);
		IndexBuffer(uint32_t* pIndices, uint32_t count);
		~IndexBuffer();

		bool SetSize(uint32_t count, bool dynamic);
		bool SetData(const void* pData);

		bool CreateBuffer(const void* pData, uint32_t count);
		void Destroy();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		uint32_t GetCount() const { return m_Count; }

	public:
		static IndexBuffer* Create(uint16_t* pIndices, uint32_t count);
		static IndexBuffer* Create(uint32_t* pIndices, uint32_t count);

	private:
		ID3D11Buffer* m_pBuffer;
		DXGI_FORMAT m_Format;
		uint32_t m_Count;
	};
}