#include "DivePch.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"

namespace Dive
{
	// 일단 default adapter로 구현
	RenderDevice::RenderDevice()
	{
		UINT deviceFlags = 0;
#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		std::vector<D3D_FEATURE_LEVEL> featureLevels
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

		auto createDevice = [this, deviceFlags, featureLevels, driverType]()
		{
			return D3D11CreateDevice(
				nullptr,
				driverType,
				nullptr,
				deviceFlags,
				featureLevels.data(),
				static_cast<UINT>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&m_D3dDevice,
				nullptr,
				&m_ImmediateContext
			);
		};

		auto result = createDevice();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			CORE_WARN("");
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDevice();
		}

		if (FAILED(result))
		{
			CORE_ERROR("");
			return;
		}
	}
	
	RenderDevice::~RenderDevice()
	{
		if (m_ImmediateContext)
		{
			m_ImmediateContext->Release();
			m_ImmediateContext = nullptr;
		}

		if (m_D3dDevice)
		{
			m_D3dDevice->Release();
			m_D3dDevice = nullptr;
		}
	}
}
