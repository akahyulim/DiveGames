#pragma once

namespace Dive
{
	// 스파키는 InputLayout도 버퍼에서 관리한다.
	// 다만 생성은 따로 함수를 구비하여 사용한다.
	// 이때 VertexShader의 blob이 필요하므로 상황이 좀 애매해진다. 
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