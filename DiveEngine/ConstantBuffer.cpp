#include "ConstantBuffer.h"
#include "Log.h"
#include "GraphicsDevice.h"
#include "Renderer.h"

namespace dive
{
	ConstantBuffer::ConstantBuffer(GraphicsDevice* graphicsDevice, const std::string& name)
		: Object(typeid(ConstantBuffer).hash_code()),
		mGraphicsDevice(graphicsDevice),
		mBuffer(nullptr),
		mName(name),
		mOffsetCount(0),
		mStride(0),
		mSizeGPU(0)
	{
		assert(graphicsDevice->IsInitialized());
	}

	ConstantBuffer::~ConstantBuffer()
	{
		destroy();
	}

	void* ConstantBuffer::Map()
	{
		if (mBuffer == nullptr)
		{
			CORE_ERROR("");
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		if (FAILED(mGraphicsDevice->GetImmediateContext()->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		{
			CORE_ERROR("");
			return nullptr;
		}

		return mappedSubresource.pData;
	}
	
	bool ConstantBuffer::Unmap()
	{
		if (mBuffer == nullptr)
		{
			CORE_ERROR("");
			return false;
		}

		mGraphicsDevice->GetImmediateContext()->Unmap(mBuffer, 0);
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
		desc.ByteWidth				= static_cast<UINT>(mStride);
		desc.MiscFlags				= 0;
		desc.StructureByteStride	= 0;

		if (FAILED(mGraphicsDevice->GetDevice()->CreateBuffer(&desc, nullptr, &mBuffer)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}
	
	void ConstantBuffer::destroy()
	{
		if (mBuffer)
		{
			mBuffer->Release();
			mBuffer = nullptr;
		}
	}
}