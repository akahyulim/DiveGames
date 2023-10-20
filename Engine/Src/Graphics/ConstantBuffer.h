#pragma once
#include "Graphics.h"
#include "Core/Object.h"
#include "Core/CoreDefs.h"
#include "Core/Log.h"

namespace Dive
{
	class ConstantBuffer : public Object
	{
	public:
		ConstantBuffer(const std::string& name);
		~ConstantBuffer() override;

		template<typename T>
		bool Create()
		{
			DV_CORE_ASSERT(Graphics::IsInitialized());
		
			m_Stride = static_cast<uint32_t>(sizeof(T));
			
			DV_RELEASE(m_pBuffer);

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = m_Stride;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			if (FAILED(Graphics::GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuffer)))
			{
				DV_CORE_ERROR("ConstantBuffer({:s}) 생성에 실패하였습니다.", m_Name);
				return false;
			}

			return true;
		}

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }

		uint32_t GetStride() const { return m_Stride; }

	private:

	private:
		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride;
	};
}