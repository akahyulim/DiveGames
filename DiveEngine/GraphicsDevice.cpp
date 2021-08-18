#include "GraphicsDevice.h"
#include "Log.h"
#include "DiveCore.h"

namespace dive
{
	/*
	* �̰� ���� �������� �� �ʿ䰡 �ֳ�?
	* Renderer���� �� �ʿ�� �ϴ� ��ü�ε�?
	* ���� ���� ������ �ʿ䰡 ���� ��ü�ε�?
	* �׷��ٸ� �����ڴ� ���ΰ� �ʱ�ȭ�� ���� Initialize �Լ��� ����� ���� ������?
	* ��? ������ ũ�Ⱑ �۴ٰ� �Ѵ�.
	*/
	GraphicsDevice::GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer)
	{
		m_bFullScreen = fullScreen;

		RECT rt;
		GetClientRect(hWnd, &rt);
		m_ResolutionWidth = static_cast<unsigned int>(rt.right - rt.left);
		m_ResolutionHeight = static_cast<unsigned int>(rt.bottom - rt.top);

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
		desc.BufferCount						= m_BackBufferCount;
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
				nullptr,									// ���� �ӽ�
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
		
		if (FAILED(result))
		{
			CORE_ERROR("Graphics Device ������ �����Ͽ����ϴ�.");
		}
	}

	GraphicsDevice::~GraphicsDevice()
	{
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pImmediateContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);
	}

	/*
	* WickedEngine�� �����Ͽ�
	* �ϴ� DepthStencilView ���� ������ ��� �����Ͽ���.
	*/
	void GraphicsDevice::PresentBegin()
	{
		m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, 0);
		
		float clearColors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColors);
	}
	
	void GraphicsDevice::PresentEnd()
	{
		m_pSwapChain->Present(static_cast<UINT>(m_bVSync), 0);
	}

	/*
	* App ���ο��� �ػ󵵸� ������ �� ȣ���Ѵ�.
	* �׷��� WickedEngine���� �̷��� ������ ����. (ResizeTarget()�� ȣ���ϴ� �ڵ� ��ü�� ����.)
	* ���� �׳� �дٸ� �̸��̶� �� �ٲٰ� �ʹ�.
	*/
	void GraphicsDevice::ResizeTarget(unsigned int width, unsigned int height)
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

	/*
	* ���� Editor�� Runtime���� �̺�Ʈ�� ó���ϸ鼭 �� �Լ��� ȣ���ϰ� �ִ�.
	*/
	void GraphicsDevice::SetResolution(unsigned int width, unsigned int height)
	{
		if ((width != m_ResolutionWidth || height != m_ResolutionHeight) && (width > 0 || height > 0))
		{
			m_ResolutionWidth = width;
			m_ResolutionHeight = height;

			DV_RELEASE(m_pBackBuffer);
			DV_RELEASE(m_pRenderTargetView);

			if (FAILED(m_pSwapChain->ResizeBuffers(m_BackBufferCount, width, height, m_Format, 0)))
			{
				CORE_ERROR("");
				return;
			}

			createBackbufferResources();
		}
	}
	
	bool GraphicsDevice::IsInitialized() const
	{
		return (m_pDevice != nullptr && m_pImmediateContext != nullptr && m_pSwapChain != nullptr);
	}

	/*
	* �ϴ� View ������ �� ������ ���� �־���.
	* ���� ������ �̰� ���⿡�� �����ϴ���, Renderer���� �����ϴ��İ� �����̴�.
	*/
	void GraphicsDevice::createBackbufferResources()
	{
		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pBackBuffer)))
		{
			CORE_ERROR("BackBuffer ������ �����Ͽ����ϴ�.");
			PostQuitMessage(0);
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(m_pBackBuffer, nullptr, &m_pRenderTargetView)))
		{
			CORE_ERROR("RenderTargetView ������ �����Ͽ����ϴ�.");
			PostQuitMessage(0);
		}
	}
}