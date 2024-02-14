#pragma once
#include "Core/CoreDefs.h"
#include "GraphicsDefs.h"
#include "Graphics.h"

namespace Dive
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer(const std::string& name, eShaderType type, uint32_t slot);
		~ConstantBuffer();

		template<typename T>
		bool CreateBuffer()
		{
			DV_RELEASE(m_pBuffer);
		
			m_Stride = static_cast<uint32_t>(sizeof(T));

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
				DV_CORE_ERROR("ConstantBuffer({:s}) 생성에 실패하였습니다.", GetName());
				return false;
			}

			return true;
		}

		void Update(void* pData);

		void Bind();

		uint32_t GetSlot() const { return m_Slot; }
		std::string GetName() const { return m_Name; }
		ID3D11Buffer* const* GetBuffer() const{ return &m_pBuffer; }
		uint32_t GetStride() const { return m_Stride; }

		template<typename T>
		static ConstantBuffer* Create(const std::string& name, eShaderType type, uint32_t slot)
		{
			auto pBuffer = new ConstantBuffer(name, type, slot);
			pBuffer->CreateBuffer<T>();

			return pBuffer;
		}

	private:
		std::string m_Name;
		eShaderType m_ShaderType;

		uint32_t m_Slot;

		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride;
	};
}