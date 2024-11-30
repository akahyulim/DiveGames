#pragma once
#include "Core/CoreDefs.h"
#include "Graphics.h"

namespace Dive
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		ConstantBuffer(const std::string& name, uint32_t stride);
		~ConstantBuffer();

		bool GenerateBuffer();
		void Release();
		
		void* Map();
		void Unmap();

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		uint32_t GetStride() const { return m_Stride; }
		void SetStride(uint32_t stride) { m_Stride = stride; }
		
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }

		static ConstantBuffer* Create(const std::string& name, uint32_t stride)
		{
			auto pNewCBuffer = new ConstantBuffer(name, stride);
			if (!pNewCBuffer->GenerateBuffer())
				DV_DELETE(pNewCBuffer);

			return pNewCBuffer;
		}

	private:
		ID3D11Buffer* m_pBuffer;
		std::string m_Name;
		uint32_t m_Stride;
	};
}