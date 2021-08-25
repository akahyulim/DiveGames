#include "GraphicsDevice.h"
#include "../Log.h"
#include "../DiveCore.h"


// 1. 생성자에 대해 좀 더 생각해본 후 정리해야 한다.
// 2. 현재 Frame Rate를 관리하지 않고 있다.
// Frame Rate 변경시에도 ResizeTarget()을 사용하는 것일까?
namespace dive
{
	GraphicsDevice::GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer)
	{
		m_bFullScreen = fullScreen;

		RECT rt;
		GetClientRect(hWnd, &rt);
		m_ResolutionWidth = static_cast<UINT>(rt.right - rt.left);
		m_ResolutionHeight = static_cast<UINT>(rt.bottom - rt.top);

		UINT deviceFlags = debugLayer ? D3D11_CREATE_DEVICE_DEBUG : 0;

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

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.OutputWindow						= hWnd;
		desc.BufferDesc.Width					= m_ResolutionWidth;
		desc.BufferDesc.Height					= m_ResolutionHeight;
		desc.Windowed							= fullScreen ? FALSE : TRUE;
		desc.BufferCount						= m_BackbufferCount;
		desc.BufferDesc.Format					= m_Format;
		desc.BufferDesc.RefreshRate.Numerator	=  60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		//desc.BufferDesc.RefreshRate.Numerator = vSync ? refreshRateNumerator : 0;
		//desc.BufferDesc.RefreshRate.Denominator = vSync ? refreshRateDenominator : 1;
		desc.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count					= 1;
		desc.SampleDesc.Quality					= 0;
		desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags								= 0;// deviceFlags;

		auto createDeviceAndSwapChain = [this, deviceFlags, featureLevels, driverType, desc]()
		{
			return D3D11CreateDeviceAndSwapChain(
				nullptr,									// 역시 임시
				driverType,
				nullptr,
				deviceFlags,
				featureLevels.data(),
				static_cast<UINT>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&desc,
				&m_pSwapChain,
				&m_pDevice,
				nullptr,
				&m_pImmediateContext);
		};

		auto result = createDeviceAndSwapChain();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			CORE_WARN("");
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDeviceAndSwapChain();
		}

		createBackbufferResource();
		
		if (FAILED(result))
		{
			CORE_ERROR("Graphics Device 생성에 실패하였습니다.");
		}
	}

	GraphicsDevice::~GraphicsDevice()
	{
		DV_RELEASE(m_pBackbufferRTV);
		DV_RELEASE(m_pBackbuffer);

		DV_RELEASE(m_pImmediateContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);
	}

	void GraphicsDevice::PresentBegin()
	{
		m_pImmediateContext->OMSetRenderTargets(1, &m_pBackbufferRTV, 0);
		
		float clearColors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_pImmediateContext->ClearRenderTargetView(m_pBackbufferRTV, clearColors);
	}
	
	void GraphicsDevice::PresentEnd()
	{
		m_pSwapChain->Present(static_cast<UINT>(m_bVSync), 0);
	}

	// 윈도우 크기 혹은 해상도를 변경할 때 사용한다.
	// ResizeResolution으로 이름을 바꾸자.
	void GraphicsDevice::ResizeTarget(UINT width, UINT height)
	{
		if ((width != m_ResolutionWidth) || (height != m_ResolutionHeight) && (width > 0) && (height > 0))
		{
			DXGI_MODE_DESC desc;
			desc.Width						= width;
			desc.Height						= height;
			desc.Format						= m_Format;
			desc.RefreshRate.Numerator		= 60;
			desc.RefreshRate.Denominator	= 1;
			desc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

			CORE_TRACE("Resize Target : {0:d} x {1:d}", width, height);

			m_pSwapChain->ResizeTarget(&desc);
		}
	}

	// WM_SIZE에 대응하여 Backbuffer 크기를 변경한다.
	// 이 함수를 이용하여 윈도우, 해상도 변경은 불가능하다.
	// ResizeBackbuffer로 이름을 바꾸자.
	// 현재 Editor의 Runtime에서 이벤트를 처리하면서 이 함수를 호출하고 있다.
	void GraphicsDevice::SetResolution(UINT width, UINT height)
	{
		if ((width != m_ResolutionWidth || height != m_ResolutionHeight) && (width > 0 || height > 0))
		{
			m_ResolutionWidth = width;
			m_ResolutionHeight = height;

			DV_RELEASE(m_pBackbuffer);
			DV_RELEASE(m_pBackbufferRTV);

			if (FAILED(m_pSwapChain->ResizeBuffers(m_BackbufferCount, width, height, m_Format, 0)))
			{
				CORE_ERROR("");
				return;
			}

			createBackbufferResource();
		}
	}
	
	bool GraphicsDevice::IsInitialized() const
	{
		return (m_pDevice != nullptr && m_pImmediateContext != nullptr && m_pSwapChain != nullptr);
	}

	void GraphicsDevice::createBackbufferResource()
	{
		DV_RELEASE(m_pBackbuffer);
		DV_RELEASE(m_pBackbufferRTV);

		DV_ASSERT(m_pSwapChain != nullptr);

		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pBackbuffer)))
		{
			CORE_ERROR("BackBuffer 생성에 실패하였습니다.");
		}

		DV_ASSERT(m_pDevice != nullptr);

		if (FAILED(m_pDevice->CreateRenderTargetView(m_pBackbuffer, nullptr, &m_pBackbufferRTV)))
		{
			CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
		}
	}
}