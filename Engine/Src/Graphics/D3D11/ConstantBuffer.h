#pragma once
#include "Core/Object.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"

namespace Dive
{
	class Context;
	class Graphics;

	class ConstantBuffer : public Object
	{
		DIVE_OBJECT(ConstantBuffer, Object);

	public:
		ConstantBuffer(Context* context);
		~ConstantBuffer();

		template<typename T>
		bool Create()
		{
			auto graphics = GetSubsystem<Graphics>();
			if (!graphics || !graphics->IsInitialized())
			{
				CORE_ERROR("");
				return false;
			}

			SAFE_RELEASE(m_buffer);

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth				= static_cast<UINT>(sizeof(T));
			desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags				= 0;
			desc.StructureByteStride	= 0;
			desc.Usage					= D3D11_USAGE_DYNAMIC;

			if (FAILED(graphics->GetRHIDevice()->CreateBuffer(&desc, nullptr, &m_buffer)))
			{
				CORE_ERROR("");
				return false;
			}
			
			return true;
		}

		void* Map();
		bool Unmap();

		ID3D11Buffer* GetBuffer() { return m_buffer; }

	private:
		ID3D11Buffer* m_buffer = nullptr;
	};
}

