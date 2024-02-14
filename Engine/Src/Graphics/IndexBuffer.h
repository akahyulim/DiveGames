#pragma once

namespace Dive
{
	// �ϴ� �̰� ����ϰ� ����ϱ⿡�� ���ϴ�.
	// IndexBuffer�� dynamic�� ���� ������ �ʱ� �����̴�.
	class IndexBuffer
	{
	public:
		IndexBuffer(uint16_t* pIndices, uint32_t count);
		IndexBuffer(uint32_t* pIndices, uint32_t count);
		~IndexBuffer();

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