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
		~ConstantBuffer() override;

		bool SetSize(unsigned int size);
		void SetParameter(unsigned int offset, unsigned int size, const void* pData);
		void Update();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer.Get(); }
		ID3D11Buffer* const* GetBufferAddressOf() const { return m_pBuffer.GetAddressOf(); }

		bool IsDirty() const { return m_bDirty; }

	private:
		unsigned char* m_pShadowData = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer = nullptr;
		unsigned int m_Stride = 0;
		bool m_bDirty = false;
	};
}
