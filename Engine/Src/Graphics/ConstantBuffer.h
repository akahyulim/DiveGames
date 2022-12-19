#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;

	class ConstantBuffer : public Object
	{
		DIVE_OBJECT(ConstantBuffer, Object)

	public:
		explicit ConstantBuffer(Context* pContext);
		~ConstantBuffer() override;

		bool SetSize(uint32_t size);
		void SetParameter(uint32_t offset, uint32_t size, const void* pData);
		void Update();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		
		bool IsDirty() const { return m_bDirty; }

	private:
		Graphics* m_pGraphics;
		uint8_t* m_pData;
		ID3D11Buffer* m_pBuffer;
		uint32_t m_Stride;
		bool m_bDirty;
	};
}
