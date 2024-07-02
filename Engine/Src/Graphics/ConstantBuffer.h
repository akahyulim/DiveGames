#pragma once
#include "Core/CoreDefs.h"
#include "Graphics.h"
#include "Shader.h"		// eShaderType 때문에... 

namespace Dive
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer(const std::string& name, eShaderType type, uint32_t index);
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

			if (FAILED(Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuffer)))
			{
				DV_ENGINE_ERROR("ConstantBuffer({:s}) 생성에 실패하였습니다.", GetName());
				return false;
			}

			return true;
		}

		void Update(void* pData);

		void Bind();

		uint32_t GetIndex() const { return m_Index; }
		std::string GetName() const { return m_Name; }
		ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
		uint32_t GetStride() const { return m_Stride; }
		eShaderType GetShaderType() const { return m_ShaderType; }

		template<typename T>
		static ConstantBuffer* Create(const std::string& name, eShaderType type, uint32_t index)
		{
			auto pBuffer = new ConstantBuffer(name, type, index);
			pBuffer->CreateBuffer<T>();

			return pBuffer;
		}

	private:
		std::string m_Name;
		eShaderType m_ShaderType;

		uint32_t m_Index;

		ID3D11Buffer* m_pBuffer;

		uint32_t m_Stride; // eVSConstBufType로 관리하는 편이 낫다. renderer.h에 선언되어 있다.
	};
}