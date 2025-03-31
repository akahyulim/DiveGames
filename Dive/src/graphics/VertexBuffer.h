#pragma once

namespace Dive
{
	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer();

		bool Create(const void* data, UINT32 stride, UINT32 count);
		void Release();

		ID3D11Buffer* GetBuffer() const { return m_Buffer; }
		UINT32 GetStride() const { return m_Stride; }
		UINT32 GetCount() const { return m_Count; }

		static std::shared_ptr<VertexBuffer> Generate(const void* data, UINT32 stride, UINT32 count);

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		UINT32 m_Stride = 0;
		UINT32 m_Count = 0;
	};
}