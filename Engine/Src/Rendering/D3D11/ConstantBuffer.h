#pragma once
#include "Core/Object.h"
#include "Core/Log.h"

namespace Dive
{
	class RenderDevice;

	class ConstantBuffer : public Object
	{
	public:
		ConstantBuffer(RenderDevice* device);
		~ConstantBuffer();

		template<typename T>
		bool Create()
		{
			D3D11_BUFFER_DESC descBuffer;
			ZeroMemory(&descBuffer, sizeof(descBuffer));
			descBuffer.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
			descBuffer.ByteWidth			= static_cast<UINT>(sizeof(T));
			descBuffer.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
			descBuffer.MiscFlags			= 0;
			descBuffer.StructureByteStride	= 0;
			descBuffer.Usage				= D3D11_USAGE_DYNAMIC;

			auto buffer_ptr = m_buffer.get();
			if (FAILED(m_device->GetD3dDevice()->CreateBuffer(&descBuffer, nullptr, &buffer_ptr)))
			{
				CORE_ERROR("ConstantBuffer::Create >> Buffer 생성에 실패하였습니다.");
				return false;
			}

			return true;
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() { return m_buffer.get(); }

	private:
		RenderDevice* m_device;
		std::shared_ptr<ID3D11Buffer> m_buffer;
	};
}

