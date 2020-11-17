#include "DivePch.h"
#include "ConstantBuffer.h"
#include "RenderDevice.h"

namespace Dive
{
	ConstantBuffer::ConstantBuffer(RenderDevice* device)
	{
		if (!device || !device->GetD3dDevice() || !device->GetImmediateContext())
		{
			CORE_ERROR("ConstantBuffer::ConstantBuffer>> RenderDevice ��ü�� ��ȿ���� �ʽ��ϴ�.");
			return;
		}

		m_device = device;
	}

	ConstantBuffer::~ConstantBuffer()
	{
	}

	void * ConstantBuffer::Map()
	{
		if (!m_buffer)
		{
			CORE_ERROR("ConstantBuffer::Map>> Buffer�� ��ȿ���� �ʽ��ϴ�.");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(m_device->GetImmediateContext()->Map(static_cast<ID3D11Resource*>(m_buffer.get()), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("ConstantBuffer::Map>> Constant Buffer Map�� �����Ͽ����ϴ�.");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool ConstantBuffer::Unmap()
	{
		if (!m_buffer)
		{
			CORE_ERROR("ConstantBuffer::Unmap>> Buffer�� ��ȿ���� �ʽ��ϴ�.");
			return false;
		}

		m_device->GetImmediateContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer.get()), 0);

		return true;
	}
}