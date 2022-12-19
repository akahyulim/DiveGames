#include "divepch.h"
#include "ConstantBuffer.h"
#include "Core/Context.h"
#include "Core/CoreDefs.h"
#include "Graphics.h"
#include "IO/Log.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(Context* pContext)
		: Object(pContext),
		m_pGraphics(GetSubsystem<Graphics>()),
		m_pData(nullptr),
		m_pBuffer(nullptr),
		m_Stride(0),
		m_bDirty(false)
	{
		DV_ASSERT(m_pGraphics->IsInitialized());
	}

	ConstantBuffer::~ConstantBuffer()
	{
		DV_DELETE_ARRAY(m_pData);
		DV_RELEASE(m_pBuffer);

		DV_LOG_ENGINE_TRACE("ConstnatBuffer 소멸 완료");
	}

	bool ConstantBuffer::SetSize(uint32_t size)
	{
		if (!size)
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::SetSize - 잘못된 크기({:d})를 전달받았습니다.", size);
			return false;
		}

		// 16바이트 단위로 변경
		size += 15;
		size &= 0xfffffff0;

		DV_DELETE_ARRAY(m_pData);

		m_Stride = size;
		m_bDirty = false;
		m_pData = new uint8_t[m_Stride];
		memset(m_pData, 0, m_Stride);

		DV_RELEASE(m_pBuffer);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = m_Stride;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		if (FAILED(m_pGraphics->GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuffer)))
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::SetSize - 버퍼 생성에 실패하였습니다.");
			return false;
		}

		return true;
	}

	void ConstantBuffer::SetParameter(uint32_t offset, uint32_t size, const void* pData)
	{
		if (offset + size > m_Stride)
			return;

		memcpy(&m_pData[offset], pData, size);
		m_bDirty = true;
	}

	void ConstantBuffer::Update()
	{
		if (m_bDirty && m_pBuffer)
		{
			m_pGraphics->GetDeviceContext()->UpdateSubresource(
				static_cast<ID3D11Resource*>(m_pBuffer), 0, 0, m_pData, 0, 0);

			m_bDirty = false;
		}
	}
}