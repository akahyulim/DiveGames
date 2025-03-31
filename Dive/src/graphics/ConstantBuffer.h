#pragma once
#include "Core/CoreDefs.h"

namespace Dive
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer() = default;
		~ConstantBuffer();

		bool GenerateBuffer();
		bool Create(UINT32 stride);
		void Release();
		
		void* Map();
		void Unmap();

		UINT32 GetStride() const { return m_Stride; }
		void SetStride(UINT32 stride) { m_Stride = stride; }
		
		ID3D11Buffer* GetBuffer() const { return m_Buffer; }

		static std::shared_ptr<ConstantBuffer> Generate(UINT32 stride);

	private:
		ID3D11Buffer* m_Buffer = nullptr;
		UINT32 m_Stride = 0;
	};
}