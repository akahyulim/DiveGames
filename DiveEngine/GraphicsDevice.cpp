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

		createBackbufferResources();
		
		if (FAILED(result))
		{
			CORE_ERROR("Graphics Device 생성에 실패하였습니다.");
		}
	}

	// BeginScene 혹은 아에 Renderer의 Render 초반으로 옮기자.
	void GraphicsDevice::PresentBegin()
	{
		auto renderTargetView = m_renderTargetView.Get();
		m_immediateContext->OMSetRenderTargets(1, &renderTargetView, 0);
		
		float clearColors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
		m_immediateContext->ClearRenderTargetView(renderTargetView, clearColors);
	}
	
	// 이건 어찌됐든 Renderer에서?
	void GraphicsDevice::PresentEnd()
	{
		m_swapChain->Present(m_bVSync, 0);
	}

	// 윈도우의 크기가 변경되었을 때 호출되어야 한다.
	void GraphicsDevice::SetResolution(unsigned int width, unsigned int height)
	{
		if ((width != m_resolutionWidth) || (height != m_resolutionHeight) && (width > 0) && (height > 0))
		{
			// render target 연결을 끊지 않았는데 문제가 없다...

			m_backBuffer.Reset();
			m_renderTargetView.Reset();

			HRESULT hr = m_swapChain->ResizeBuffers(m_backBufferCount, width, height, m_format, 0);
			assert(SUCCEEDED(hr));

			m_resolutionWidth = width;
			m_resolutionHeight = height;

			createBackbufferResources();
		}
	}
	
	bool GraphicsDevice::IsInitialized() const
	{
		return (m_device != nullptr && m_immediateContext != nullptr && m_swapChain != nullptr);
	}

	void GraphicsDevice::BindViewports(unsigned int count, const D3D11_VIEWPORT* viewports)
	{
		assert(count < 6);

		D3D11_VIEWPORT vps[6];
		for (unsigned int i = 0; i < count; i++)
		{
			vps[i].TopLeftX = viewports[i].TopLeftX;
			vps[i].TopLeftY = viewports[i].TopLeftY;
			vps[i].Width	= viewports[i].Width;
			vps[i].Height	= viewports[i].Height;
			vps[i].MinDepth = viewports[i].MinDepth;
			vps[i].MaxDepth = viewports[i].MaxDepth;
		}

		m_immediateContext->RSSetViewports(count, vps);
	}

	void GraphicsDevice::createBackbufferResources()
	{
		if (FAILED(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBuffer)))
		{
			CORE_ERROR("BackBuffer 생성에 실패하였습니다.");
			PostQuitMessage(0);
		}

		if (FAILED(m_device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_renderTargetView)))
		{
			CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
			PostQuitMessage(0);
		}
	}
}