#pragma once

namespace Dive
{
	// �ϴ� �̰� ����ϰ� ����ϱ⿡�� ���ϴ�.
	// IndexBuffer�� dynamic�� ���� ������ �ʱ� �����̴�.
	class DvIndexBuffer
	{
	public:
		DvIndexBuffer(uint16_t* pIndices, uint32_t count);
		DvIndexBuffer(uint32_t* pIndices, uint32_t count);
		~DvIndexBuffer();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		DXGI_FORMAT GetFormat() const { return m_Format; }
		uint32_t GetCount() const { return m_Count; }

	public:
		static DvIndexBuffer* Create(uint16_t* pIndices, uint32_t count);
		static DvIndexBuffer* Create(uint32_t* pIndices, uint32_t count);

	private:
		ID3D11Buffer* m_pBuffer;
		DXGI_FORMAT m_Format;
		uint32_t m_Count;
	};
}