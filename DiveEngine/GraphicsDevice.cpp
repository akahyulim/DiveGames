#include "GraphicsDevice.h"
#include "Log.h"
#include "DiveCore.h"

namespace dive
{
	/*
	* 이걸 굳이 동적생성 할 필요가 있나?
	* Renderer에서 꼭 필요로 하는 객체인데?
	* 실행 도중 제거할 필요가 없는 객체인데?
	* 그렇다면 생성자는 놔두고 초기화를 위해 Initialize 함수를 만드는 편이 낫겠지?
	* 잉? 스택은 크기가 작다고 한다.
	*/
	GraphicsDevice::GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer)
	{
		m_bFullScreen = fullScreen;

		RECT rt;
		GetClientRect(hWnd, &rt);
		m_Width = static_cast<unsigned int>(rt.right - rt.left);
		m_Height = static_cast<unsigned int>(rt.bottom - rt.top);

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
		desc.BufferDesc.Width					= m_Width;
		desc.BufferDesc.Height					= m_Height;
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
		
		if (FAILED(result))
		{
			CORE_ERROR("Graphics Device 생성에 실패하였습니다.");
		}
	}

	GraphicsDevice::~GraphicsDevice()
	{
		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pDepthStencilBuffer);
		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pImmediateContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);
	}

	/*
	* DepthStencilView가 꼭 필요한지 의문이다.
	* 바로 아래 PresentEnd()와 함께 사용한다.
	* 현재 Runtime::Render()에서 호출된다.
	* => Render의 시작과 끝에서 BackBuffer를 RenderTarget으로 둘 이유가 없다.
	* => 물론 Editor의 경우엔 다르다.
	* 아무리봐도 이건 Renderer에서 구현되어야 할 것 같다.
	* 그리고 DepthStencilView의 생성 및 관리도 GraphicsDevice에는 어울리지 않는 것 같다.
	* 따지고보면 BackBuffer RTV도 그냥 Renderer에서 생성 및 관리하는 편이 나을 것 같기도...
	*/
	void GraphicsDevice::PresentBegin()
	{
		m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
		
		float clearColors[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColors);
		m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	
	void GraphicsDevice::PresentEnd()
	{
		m_pSwapChain->Present(m_bVSync ? 1 : 0, 0);
	}

	/*
	* 현재 Editor의 Runtime에서 이벤트를 처리하면서 이 함수를 호출하고 있다.
	*/
	void GraphicsDevice::ResizeBuffers(unsigned int width, unsigned int height)
	{
		if (width < 0 || height < 0)
		{
			CORE_ERROR("Receive wrong size.");
			return;
		}

		if (width == m_Width && height == m_Height)
			return;

		m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

		DV_RELEASE(m_pRenderTargetView);
		DV_RELEASE(m_pDepthStencilBuffer);
		DV_RELEASE(m_pDepthStencilView);

		if (!SUCCEEDED(m_pSwapChain->ResizeBuffers(m_BackBufferCount, width, height, m_Format, 0)))
		{
			CORE_ERROR("Fail to Reisze Bakbuffer.");
			return;
		}

		m_Width = width;
		m_Height = height;

		createBackbufferResources();

		CORE_TRACE("Resize Resolution : {0:d} x {1:d}", m_Width, m_Height);
	}

	//==========================================================================================//
	// App 내부에서 해상도를 변경할 때 사용한다.												//
	// ResizeResolution과 이름의 통일성이 없다.													//
	//==========================================================================================//
	void GraphicsDevice::ResizeTarget(unsigned int width, unsigned int height)
	{
		if ((width != m_Width) || (height != m_Height) && (width > 0) && (height > 0))
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
	
	bool GraphicsDevice::IsInitialized() const
	{
		return (m_pDevice != nullptr && m_pImmediateContext != nullptr && m_pSwapChain != nullptr);
	}

	/*
	* 일단 View 생성을 한 군데에 몰아 넣었다.
	* 이제 문제는 이걸 여기에서 관리하느냐, Renderer에서 관리하느냐가 문제이다.
	*/
	void GraphicsDevice::createBackbufferResources()
	{
		ID3D11Texture2D* pBackBuffer = nullptr;
		
		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
		{
			CORE_ERROR("BackBuffer 생성에 실패하였습니다.");
			PostQuitMessage(0);
		}

		if (FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView)))
		{
			CORE_ERROR("RenderTargetView 생성에 실패하였습니다.");
			PostQuitMessage(0);
		}

		DV_RELEASE(pBackBuffer);

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pDepthStencilBuffer)))
		{
			CORE_ERROR("DepthStencil TextureBuffer 생성 실패");
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(viewDesc));
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &viewDesc, &m_pDepthStencilView)))
		{
			CORE_ERROR("DepthStencilView 생성 실패");
		}
	}
}