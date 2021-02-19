#pragma once
#include "Core/Object.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"
#include "../Graphics.h"

namespace Dive
{
	class Context;
	class Graphics;

	class ConstantBuffer : public Object
	{
		DIVE_OBJECT(ConstantBuffer, Object);

	public:
		// dynamic 여부와 이름을 받는다.
		// dx11에는 필요없다고 한다.
		ConstantBuffer(Context* context);
		~ConstantBuffer();

		// offset count를 전달 받는다. 스왑체인버퍼 개수이다...
		// 이 offset count와 m_strid(sizeof(T))가 size가 된다. 즉, 버퍼 크기를 계산하는 용도 외에는 쓸모 없다.
		// 아래 구현 사항에는 변동이 없다.
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

		// offset index와 offse dynamic index라는걸 가진다.
	};
}

