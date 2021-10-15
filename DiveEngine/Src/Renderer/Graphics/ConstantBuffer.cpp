#include "ConstantBuffer.h"
#include "../../Helper/Log.h"
#include "GraphicsDevice.h"
#include "../Renderer.h"

namespace dive
{
	ConstantBuffer::ConstantBuffer(GraphicsDevice* pGraphicsDevice, const std::string& name)
		: m_pGraphicsDevice(pGraphicsDevice),
		m_pBuffer(nullptr),
		m_Name(name),
		m_OffsetCount(0),
		m_Stride(0),
		m_SizeGPU(0)
	{
		assert(pGraphicsDevice->IsInitialized());
	}

	ConstantBuffer::~ConstantBuffer()
	{
		destroy();
	}

	void* ConstantBuffer::Map()
	{
		if (m_pBuffer == nullptr)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		if (FAILED(m_pGraphicsDevice->GetImmediateContext()->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedSubresource.pData;
	}
	
	bool ConstantBuffer::Unmap()
	{
		if (m_pBuffer == nullptr)
		{
			CORE_ERROR("");
			return false;
		}

		m_pGraphicsDevice->GetImmediateContext()->Unmap(m_pBuffer, 0);
		return true;
	}
	
	bool ConstantBuffer::create()
	{
		destroy();

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage					= D3D11_USAGE_DYNAMIC;
		desc.BindFlags				= D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags			= D3D11_CPU_ACCESS_WRITE;
		desc.ByteWidth				= static_cast<UINT>(m_Stride);
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;

		if (FAILED(m_pGraphicsDevice->GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
	
	void ConstantBuffer::destroy()
	{
		if (m_pBuffer)
		{
			m_pBuffer->Release();
			m_pBuffer = nullptr;
		}
	}
}