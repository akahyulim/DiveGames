#pragma once

namespace Dive
{
	// ����Ű�� InputLayout�� ���ۿ��� �����Ѵ�.
	// �ٸ� ������ ���� �Լ��� �����Ͽ� ����Ѵ�.
	// �̶� VertexShader�� blob�� �ʿ��ϹǷ� ��Ȳ�� �� �ָ�������. 
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* pData, uint32_t stride, uint32_t count);
		~VertexBuffer();

		ID3D11Buffer* const* GetBuffer() const { return &m_pBuffer; }
		const uint32_t* GetStride() const { return &m_Stride; }
		const uint32_t* GetOffset() const { return &m_Offset; }
		uint32_t GetCount() const { return m_Count; }

	public:
		static VertexBuffer* Create(const void* pData, uint32_t stride, uint32_t count);

	private:
		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride;
		uint32_t m_Offset;
		uint32_t m_Count;
	};
}