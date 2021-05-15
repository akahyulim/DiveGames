#include "GraphicsDevice.h"
#include "Log.h"

namespace Dive
{
	// debugLayer에 맞춰 디버그 데이터 출력도 염두해 둘려면
	// 해당 매개변수를 저장하는게 낫다.
	GraphicsDevice::GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer)
	{
		m_bFullScreen = fullScreen;

		RECT rt;
		GetClientRect(hWnd, &rt);
		m_resolutionWidth = static_cast<unsigned int>(rt.right - rt.left);
		m_resolutionHeight = static_cast<unsigned int>(rt.bottom - rt.top);

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
		desc.OutputWindow					= hWnd;
		desc.BufferDesc.Width				= m_resolutionWidth;
		desc.BufferDesc.Height				= m_resolutionHeight;
		desc.Windowed						= fullScreen ? FALSE : TRUE;
		desc.BufferCount					= m_backBufferCount;
		desc.BufferDesc.Format				= m_format;
		//desc.BufferDesc.RefreshRate.Numerator = vSync ? refreshRateNumerator : 0;
		//desc.BufferDesc.RefreshRate.Denominator = vSync ? refreshRateDenominator : 1;
		desc.BufferDesc.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling				= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage					= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count				= 1;
		desc.SampleDesc.Quality				= 0;
		desc.SwapEffect						= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags							= deviceFlags;

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

		createBackbufferResources();
		createDepthStencilView();
		
		if (FAILED(result))
		{
			CORE_ERROR("Graphics Device 생성에 실패하였습니다.");
		}
	}

	// BeginScene 혹은 아에 Renderer의 Render 초반으로 옮기자.
	// => 잘 모르겠다....
	void GraphicsDevice::PresentBegin()
	{
		m_pImmediateContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());
		
		float clearColors[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		m_pImmediateContext->ClearRenderTargetView(m_pRTV.Get(), clearColors);
		m_pImmediateContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	
	// 이건 어찌됐든 Renderer에서?
	void GraphicsDevice::PresentEnd()
	{
		m_pSwapChain->Present(m_bVSync ? 1 : 0, 0);
	}

	// 윈도우의 크기가 변경되었을 때 호출되어야 한다.
	void GraphicsDevice::SetResolution(unsigned int width, unsigned int height)
	{
		if ((width != m_resolutionWidth) || (height != m_resolutionHeight) && (width > 0) && (height > 0))
		{
			// render target 연결을 끊지 않았는데 문제가 없다...

			m_pBackBuffer.Reset();
			m_pRTV.Reset();

			HRESULT hr = m_pSwapChain->ResizeBuffers(m_backBufferCount, width, height, m_format, 0);
			assert(SUCCEEDED(hr));

			m_resolutionWidth = width;
			m_resolutionHeight = height;

			createBackbufferResources();
		}
	}
	
	bool GraphicsDevice::IsInitialized() const
	{
		return (m_pDevice != nullptr && m_pImmediateContext != nullptr && m_pSwapChain != nullptr);
	}

	void GraphicsDevice::createBackbufferResources()
	{
		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_pBackBuffer)))
		{
			CORE_ERROR("BackBuffer 생성에 실패하였습니다.");
			PostQuitMessage(0);
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr, &m_pRTV)))
		{
			CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
			PostQuitMessage(0);
		}
	}

	void GraphicsDevice::createDepthStencilView()
	{
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));

		// Set up the description of the depth buffer.
		texDesc.Width = GetResolutionWidth();
		texDesc.Height = GetResolutionHeight();
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, m_pDSBuffer.GetAddressOf())))
		{
			CORE_ERROR("DepthStencil TextureBuffer 생성 실패");
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(viewDesc));
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pDSBuffer.Get(), &viewDesc, m_pDSV.GetAddressOf())))
		{
			CORE_ERROR("DepthStencilView 생성 실패");
		}
	}
}