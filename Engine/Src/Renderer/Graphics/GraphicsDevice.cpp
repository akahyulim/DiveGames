#include "divepch.h"
#include "GraphicsDevice.h"
#include "Base/Base.h"
#include "Base/Engine.h"

namespace Dive
{
	void GraphicsDevice::Initialize(const WindowData* pData)
	{
		DV_ASSERT(pData != nullptr);

		m_hWnd = pData->hWnd;
		m_Width = pData->Width;
		m_Height = pData->Height;
		m_bVSync = pData->bVSync;
		m_bFullScreen = pData->bFullScreen;

		UINT deviceFlags = 0;

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
		desc.OutputWindow = m_hWnd;
		desc.BufferDesc.Width = m_Width;
		desc.BufferDesc.Height = m_Height;
		desc.Windowed = m_bFullScreen ? FALSE : TRUE;
		desc.BufferCount = m_BackbufferCount;
		desc.BufferDesc.Format = m_Format;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		//desc.BufferDesc.RefreshRate.Numerator = m_bVSync ? refreshRateNumerator : 0;
		//desc.BufferDesc.RefreshRate.Denominator = m_bVSync ? refreshRateDenominator : 1;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = deviceFlags;

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
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDeviceAndSwapChain();
		}

		DV_ASSERT(result == S_OK);

		createMainRenderTargetView();
	}

	void GraphicsDevice::Shutdown()
	{
		DV_RELEASE(m_pMainRenderTargetView);
		DV_RELEASE(m_pImmediateContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);
	}

	void GraphicsDevice::ResizeBackBuffer(unsigned int width, unsigned int height)
	{
		DV_ASSERT(m_pSwapChain != nullptr);
		DV_ASSERT(m_pImmediateContext != nullptr);

		if (width == m_Width && height == m_Height)
			return;

		m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
		DV_RELEASE(m_pMainRenderTargetView);

		if (FAILED(m_pSwapChain->ResizeBuffers(m_BackbufferCount, width, height, m_Format, 0)))
		{
			DV_CORE_WARN("ResizeBuffer를 실패하였습니다.");
			return;
		}

		createMainRenderTargetView();

		m_Width = width;
		m_Height = height;
	}

	// 일단 기억을 위해 남겨둔다.
	// 1. 결국 SetRenderTargetView와 분리된 형태이다.
	// 2. 결국 단순 랩핑 함수이다.
	void GraphicsDevice::ClearRenderTargetView(ID3D11RenderTargetView* pRenderTargetView, const float* pColors)
	{
		DV_ASSERT(m_pImmediateContext != nullptr);
		DV_ASSERT(pRenderTargetView != nullptr);

		float defaultColors[4] = {1.0f, 1.0f, 1.0f, 1.0f};

		m_pImmediateContext->ClearRenderTargetView(
			pRenderTargetView,
			(pColors != nullptr) ? pColors : defaultColors);
	}

	void GraphicsDevice::Present()
	{
		DV_ASSERT(m_pSwapChain != nullptr);

		m_pSwapChain->Present(m_bVSync ? 0 : 1, 0);
	}

	void GraphicsDevice::createMainRenderTargetView()
	{
		DV_ASSERT(m_pSwapChain != nullptr);
		DV_ASSERT(m_pDevice != nullptr);

		DV_RELEASE(m_pMainRenderTargetView);

		ID3D11Texture2D* pBackbuffer = nullptr;
		m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackbuffer);

		if (FAILED(m_pDevice->CreateRenderTargetView(pBackbuffer, NULL, &m_pMainRenderTargetView)))
		{
			DV_CORE_WARN("MainRenderTargetView 생성에 실패하였습니다.");
		}

		DV_RELEASE(pBackbuffer);
	}
}