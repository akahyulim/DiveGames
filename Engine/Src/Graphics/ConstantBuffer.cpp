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

	void ConstantBuffer::Update(void* pData)
	{
		auto pGraphics = GetSubsystem<Graphics>();
		if (!pGraphics || !pGraphics->IsInitialized())
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::Update - Graphics system�� ����� �� �����ϴ�.");
			return;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(pGraphics->GetDeviceContext()->Map(
			static_cast<ID3D11Resource*>(m_pBuffer.Get()), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::Update - Mapping�� �����Ͽ����ϴ�.");
			return;
		}

		memcpy(reinterpret_cast<std::byte*>(mappedResource.pData), reinterpret_cast<std::byte*>(pData), m_Stride);

		pGraphics->GetDeviceContext()->Unmap(static_cast<ID3D11Resource*>(m_pBuffer.Get()), 0);
	}

	bool ConstantBuffer::create()
	{
		m_pBuffer.Reset();

		if (!m_Stride)
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::create - �߸��� ũ�⸦ ���޹޾� ���۸� ������ �� �����ϴ�.");
			return false;
		}

		auto pGraphics = GetSubsystem<Graphics>();
		if (!pGraphics || !pGraphics->IsInitialized())
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::create - Graphics system�� ����� �� �����ϴ�.");
			return false;
		}

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = m_Stride;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		if (FAILED(pGraphics->GetDevice()->CreateBuffer(&desc, nullptr, m_pBuffer.GetAddressOf())))
		{
			DV_LOG_ENGINE_ERROR("ConstantBuffer::create - ���� ������ �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}
}