#include "DivePch.h"
#include "DeviceAndSwapChain.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	DeviceAndSwapChain::DeviceAndSwapChain(Context* context, HWND hWnd, unsigned int width, unsigned int height, bool windowed,
		unsigned int bufferCount, DXGI_FORMAT format, bool vSync, unsigned int refreshRateNumerator, unsigned int refreshRateDenominator)
		: Object(context),
		m_device(nullptr),
		m_immediateContext(nullptr),
		m_swapChain(nullptr),
		m_renderTargetView(nullptr),
		m_bufferCount(0),
		m_bInitialized(false)
	{
		if (!hWnd) 
		{
			CORE_ERROR("");
			return; 
		}

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

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.OutputWindow						= hWnd;
		desc.BufferDesc.Width					= width;
		desc.BufferDesc.Height					= height;
		desc.Windowed							= windowed ? TRUE : FALSE;
		desc.BufferCount						= bufferCount;
		desc.BufferDesc.Format					= format;
		desc.BufferDesc.RefreshRate.Numerator	= vSync ? refreshRateNumerator : 0;
		desc.BufferDesc.RefreshRate.Denominator	= vSync ? refreshRateDenominator : 1;
		desc.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count					= 1;
		desc.SampleDesc.Quality					= 0;
		desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags								= 0;

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
				&m_swapChain,
				&m_device,
				nullptr,
				&m_immediateContext);
		};

		auto result = createDeviceAndSwapChain();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			CORE_WARN("");
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDeviceAndSwapChain();
		}

		if (FAILED(result))
		{
			CORE_ERROR("");
			return;
		}

		if (!createRenderTargetView())
			return;

		m_size.x		= width;
		m_size.y		= height;
		m_bufferCount	= bufferCount;
		m_format		= format;
		m_bWindowed		= windowed;
		m_bVsyncEnabled = vSync;
		m_refreshRate.x = refreshRateDenominator;
		m_refreshRate.y = refreshRateNumerator;

		m_bInitialized	= true;

		CORE_TRACE("Created d3d11 rhi.");
	}

	DeviceAndSwapChain::~DeviceAndSwapChain()
	{
		SAFE_RELEASE(m_swapChain);
		SAFE_RELEASE(m_immediateContext);
		SAFE_RELEASE(m_device);
	}

	void DeviceAndSwapChain::Present()
	{
		if (!IsInitialized())
			return;

		m_swapChain->Present(m_bVsyncEnabled ? 1 : 0, 0);
	}

	// 윈도우의 WM_RESIZE 발생시 호출됩니다.
	// width = 0, height = 0일 경우 윈도우 크기에 맞춰 집니다.
	bool DeviceAndSwapChain::ResizeBuffer(DirectX::XMUINT2 size)
	{
		if (!IsInitialized())
		{
			CORE_ERROR("");
			return false;
		}

		// 결국 이 부분이 문제였다.
		// RenderTargetView들을 무조건 해제해야 하는 것 같다.
//		bool used = false;
//		ID3D11RenderTargetView* currentRenderTargetView = nullptr;
//		ID3D11DepthStencilView* currentDepthStencilView = nullptr;
//		m_immediateContext->OMGetRenderTargets(1, &currentRenderTargetView, &currentDepthStencilView);
//		if (m_renderTargetView == currentRenderTargetView)
		{
			m_immediateContext->OMSetRenderTargets(0, nullptr, nullptr);
//			used = true;
		}

		// 꼭 미리 릴리즈 시켜야 한다.
		SAFE_RELEASE(m_renderTargetView);

		if(FAILED(m_swapChain->ResizeBuffers(0, size.x, size.y, DXGI_FORMAT_UNKNOWN, 0)))
		{
//			if (used)
			{
//				m_immediateContext->OMSetRenderTargets(1, &m_renderTargetView, currentDepthStencilView);
			}

			CORE_ERROR("fail to resize buffers.");
			return false;
		}

		// depth stencil view도 새로 생성해야할 수 있다.
		if (!createRenderTargetView())
		{
			return false;
		}

//		if (used)
		{
//			m_immediateContext->OMSetRenderTargets(1, &m_renderTargetView, currentDepthStencilView);
		}

		DXGI_SWAP_CHAIN_DESC desc;
		m_swapChain->GetDesc(&desc);

		CORE_TRACE("Change Resolution Width: {0:d} -> {1:d}", m_size.x, desc.BufferDesc.Width);
		CORE_TRACE("Change Resolution Height: {0:d} -> {1:d}", m_size.y, desc.BufferDesc.Height);

		m_size = DirectX::XMUINT2(desc.BufferDesc.Width, desc.BufferDesc.Height);

		return true;
	}

	bool DeviceAndSwapChain::ResizeTarget(DirectX::XMUINT2 size, bool force)
	{
		if (!IsInitialized())
		{
			CORE_ERROR("");
			return false;
		}

		if (!force)
		{
			if (m_size.x == size.x && m_size.y == size.y)
			{
				CORE_TRACE("");
				return true;
			}
		}

		DXGI_MODE_DESC desc;
		desc.Width						= size.x;
		desc.Height						= size.y;
		desc.RefreshRate.Denominator	= m_refreshRate.x;
		desc.RefreshRate.Numerator		= m_refreshRate.y;
		desc.Format						= m_format;
		desc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

		if (FAILED(m_swapChain->ResizeTarget(&desc)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	// 일단 OutputTarget을 NULL로 설정
	// 윈도우는 자동으로 바뀌나? 아닐거 같은데...
	// Graphics에서 Window와 SwapChain 둘 다 다뤄야할 것 같다. 
	void DeviceAndSwapChain::SetFullScreen(bool fullScreen)
	{
		if (!IsInitialized())
			return;
		
		m_swapChain->SetFullscreenState(fullScreen, NULL);
	}

	bool DeviceAndSwapChain::createRenderTargetView()
	{
		SAFE_RELEASE(m_renderTargetView);

		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return false;
		}

		if (FAILED(m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView)))
		{
			CORE_ERROR("");
			return false;
		}
		backBuffer->Release();
		
		return true;
	}
}