#include "divepch.h"
#include "ConstantBuffer.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Graphics.h"
#include "IO/Log.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(Context* pContext)
		: Object(pContext)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		DV_LOG_ENGINE_DEBUG("ConstnatBuffer 소멸자 호출");
	}

	bool ConstantBuffer::SetSize(unsigned int size)
	{
		m_pBuffer.Reset();

		if (!size)
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::SetSize - 잘못된 크기({:d})를 전달받았습니다.", size);
			return false;
		}

		// 16바이트 단위로 변경
		size += 15;
		size &= 0xfffffff0;

		m_Stride = size;
		m_bDirty = false;
		m_pShadowData = new unsigned char[m_Stride];
		memset(m_pShadowData, 0, m_Stride);

		auto pGraphics = GetSubsystem<Graphics>();
		if (!pGraphics || !pGraphics->IsInitialized())
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::SetSize - Graphics system을 사용할 수 없습니다.");
			return false;
		}

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = m_Stride;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		if (FAILED(pGraphics->GetDevice()->CreateBuffer(&desc, nullptr, m_pBuffer.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::SetSize - 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	void ConstantBuffer::SetParameter(unsigned int offset, unsigned int size, const void* pData)
	{
		if (offset + size > m_Stride)
			return;

		memcpy(&m_pShadowData[offset], pData, size);
		m_bDirty = true;
	}

	void ConstantBuffer::Update()
	{
		if (m_bDirty && m_pBuffer)
		{
			auto pGraphics = GetSubsystem<Graphics>();
			if (!pGraphics || !pGraphics->IsInitialized())
			{
				DV_LOG_ENGINE_ERROR("ConstantBuffer::Update - Graphics system을 사용할 수 없습니다.");
				return;
			}

			pGraphics->GetDeviceContext()->UpdateSubresource(
				static_cast<ID3D11Resource*>(m_pBuffer.Get()), 0, 0, m_pShadowData, 0, 0);

			m_bDirty = false;
		}
	}
}