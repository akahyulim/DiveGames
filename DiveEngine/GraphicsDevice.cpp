#include "GraphicsDevice.h"
#include "Log.h"

namespace dive
{
	// debugLayer�� ���� ����� ������ ��µ� ������ �ѷ���
	// �ش� �Ű������� �����ϴ°� ����.
	GraphicsDevice::GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer)
	{
		mbFullScreen = fullScreen;

		RECT rt;
		GetClientRect(hWnd, &rt);
		mResolutionWidth = static_cast<unsigned int>(rt.right - rt.left);
		mResolutionHeight = static_cast<unsigned int>(rt.bottom - rt.top);

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
		desc.BufferDesc.Width				= mResolutionWidth;
		desc.BufferDesc.Height				= mResolutionHeight;
		desc.Windowed						= fullScreen ? FALSE : TRUE;
		desc.BufferCount					= mBackBufferCount;
		desc.BufferDesc.Format				= mFormat;
		desc.BufferDesc.RefreshRate.Numerator =  60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		//desc.BufferDesc.RefreshRate.Numerator = vSync ? refreshRateNumerator : 0;
		//desc.BufferDesc.RefreshRate.Denominator = vSync ? refreshRateDenominator : 1;
		desc.BufferDesc.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling				= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage					= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count				= 1;
		desc.SampleDesc.Quality				= 0;
		desc.SwapEffect						= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0;// deviceFlags;

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
				&mSwapChain,
				&mDevice,
				nullptr,
				&mImmediateContext);
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
			CORE_ERROR("Graphics Device ������ �����Ͽ����ϴ�.");
		}
	}

	//==========================================================================================//
	// DepthStencilView�� �����ϸ� ResizeResolution()�� ������ �ʴ´�.							//
	// �Ƹ��� DetphStencilView�� �ٽ� �����ؾ��ϳ� ����.										//
	// => �� ������ ���� ResizeResolution()���� �ӽ÷� �ذ��ߴ�.								//
	//==========================================================================================//
	void GraphicsDevice::PresentBegin()
	{
		//mImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), nullptr);
		mImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
		
		float clearColors[4] = { 0.25f, 0.25f, 0.25f, 1.0f };
		mImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColors);
		mImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}
	
	// �̰� ����Ƶ� Renderer����?
	void GraphicsDevice::PresentEnd()
	{
		mSwapChain->Present(mbVSync ? 1 : 0, 0);
	}

	//==========================================================================================//
	// ���� ȣ���� ������ ���� ������ ���ߴ�.													//
	// RenderTarget ������ ���� �ʾҴ�.															//
	// WM_SIZE���� Minimize�� 0, 0�� �����Ѵ�.													//
	//==========================================================================================//
	void GraphicsDevice::ResizeBuffers(unsigned int width, unsigned int height)
	{
		if ((width != mResolutionWidth) || (height != mResolutionHeight))// && (width > 0) && (height > 0))
		{
			//mImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

			mBackBuffer.Reset();
			mRenderTargetView.Reset();

			HRESULT hr = mSwapChain->ResizeBuffers(mBackBufferCount, width, height, mFormat, 0);
			assert(SUCCEEDED(hr));

			mResolutionWidth = width;
			mResolutionHeight = height;

			createBackbufferResources();

			CORE_TRACE("Resize Resolution : {0:d} x {1:d}", mResolutionWidth, mResolutionHeight);

			//===========================================================================================
			// DepthStencilBuffer�� �ӽ÷� ���⿡�� ������ �� ȣ��
			mDepthStencilBuffer.Reset();
			mDepthStencilView.Reset();

			createDepthStencilView();
			//============================================================================================
		}
	}

	//==========================================================================================//
	// App ���ο��� �ػ󵵸� ������ �� ����Ѵ�.												//
	// ResizeResolution�� �̸��� ���ϼ��� ����.													//
	//==========================================================================================//
	void GraphicsDevice::ResizeTarget(unsigned int width, unsigned int height)
	{
		if ((width != mResolutionWidth) || (height != mResolutionHeight) && (width > 0) && (height > 0))
		{
			DXGI_MODE_DESC desc;
			desc.Width						= width;
			desc.Height						= height;
			desc.Format						= mFormat;
			desc.RefreshRate.Numerator		= 60;
			desc.RefreshRate.Denominator	= 1;
			desc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

			CORE_TRACE("Resize Target : {0:d} x {1:d}", width, height);

			mSwapChain->ResizeTarget(&desc);
		}
	}
	
	bool GraphicsDevice::IsInitialized() const
	{
		return (mDevice != nullptr && mImmediateContext != nullptr && mSwapChain != nullptr);
	}

	//==========================================================================================//
	// �̸��� create�� ���ԵǾ� �ִٸ� �� ���� ���� ������� �Ѵ�.							//
	// 1. �̹� �����Ǿ� �ִٸ� pass																//
	// 2. �̹� �����Ǿ� �ִٸ� ������ ���� �ٽ� ����											//
	// �� �κ��� �� �� �����غ��� �Ѵ�.															//
	//==========================================================================================//
	void GraphicsDevice::createBackbufferResources()
	{
		if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &mBackBuffer)))
		{
			CORE_ERROR("BackBuffer ������ �����Ͽ����ϴ�.");
			PostQuitMessage(0);
		}

		if (FAILED(mDevice->CreateRenderTargetView(mBackBuffer.Get(), nullptr, &mRenderTargetView)))
		{
			CORE_ERROR("RenderTargetView ������ �����Ͽ����ϴ�.");
			PostQuitMessage(0);
		}
	}

	//==========================================================================================//
	// OMSetRenderTargetViews()�� �����Ϸ���													//
	// ResizeResolution()�� �ٽ� �����ؾ� �� �� ����.											//
	// ���� ResizeResolution()���� Reset�� �ٽ� �� �Լ� ȣ����� �����ߴ�.						//
	//==========================================================================================//
	void GraphicsDevice::createDepthStencilView()
	{
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
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

		if (FAILED(mDevice->CreateTexture2D(&texDesc, nullptr, mDepthStencilBuffer.GetAddressOf())))
		{
			CORE_ERROR("DepthStencil TextureBuffer ���� ����");
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(viewDesc));
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &viewDesc, mDepthStencilView.GetAddressOf())))
		{
			CORE_ERROR("DepthStencilView ���� ����");
		}
	}
}