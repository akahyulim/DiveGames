#pragma once
#include "GraphicsDevice.h"
#include "Renderer.h"
#include "Log.h"

namespace Dive
{
	// buffer ����, ������ Ÿ��, dynamic ���� ������???
	class ConstantBuffer
	{
	public:

		template<typename T>
		bool Create()
		{
			auto device = Renderer::GetInstance().GetDevice()->GetDevice();
			assert(device != nullptr);

			// �̰� �⺻������ dynamic�̹Ƿ� data�� ����.
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