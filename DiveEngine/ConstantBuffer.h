#pragma once
#include "GraphicsDevice.h"
#include "Renderer.h"
#include "Log.h"

namespace Dive
{
	// buffer 종류, 데이터 타입, dynamic 유무 등으로???
	class ConstantBuffer
	{
	public:

		template<typename T>
		bool Create()
		{
			auto device = Renderer::GetInstance().GetDevice()->GetDevice();
			assert(device != nullptr);

			// 이건 기본적으로 dynamic이므로 data가 없다.
			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			if (FAILED(device->CreateBuffer(&desc, nullptr, &m_buffer)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		void* Map();
		void Unmap();

		ID3D11Buffer* GetBuffer() { return m_buffer.Get(); }

	private:
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	};
}