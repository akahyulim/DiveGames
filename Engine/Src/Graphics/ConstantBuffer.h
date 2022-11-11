#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;

	class ConstantBuffer : public Object
	{
		DIVE_OBJECT(ConstantBuffer, Object)

	public:
		explicit ConstantBuffer(Context* pContext);
		~ConstantBuffer() override = default;

		template<typename T> bool CreateBuffer();

		void Update(void* pData);

		ID3D11Buffer* GetBuffer() const { return m_pBuffer.Get(); }
		ID3D11Buffer* const* GetBufferAddressOf() const { return m_pBuffer.GetAddressOf(); }

	private:
		bool create();

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer = nullptr;
		unsigned int m_Stride = 0;
	};

	template<typename T>
	bool ConstantBuffer::CreateBuffer()
	{
		m_Stride = static_cast<unsigned int>(sizeof(T));

		return create();
	}
}
