#include "DivePch.h"
#include "ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Core/Context.h"


namespace Dive
{
	ConstantBuffer::ConstantBuffer(Context* context)
		: Object(context)
	{
	}

	ConstantBuffer::~ConstantBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	void * ConstantBuffer::Map()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || !m_buffer)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(graphics->GetRHIContext()->Map(static_cast<ID3D11Resource*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedResource.pData;
	}

	bool ConstantBuffer::Unmap()
	{
		auto graphics = GetSubsystem<Graphics>();
		if (!graphics || !graphics->IsInitialized() || !m_buffer)
		{
			CORE_ERROR("");
			return false;
		}

		graphics->GetRHIContext()->Unmap(static_cast<ID3D11Resource*>(m_buffer), 0);

		return true;
	}
}