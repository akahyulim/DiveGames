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

		CreateMainRenderTargetView();
	}

	void GraphicsDevice::Shutdown()
	{
		DV_RELEASE(m_pMainRenderTargetView);
		DV_RELEASE(m_pImmediateContext);
		DV_RELEASE(m_pDevice);
		DV_RELEASE(m_pSwapChain);
	}

	void GraphicsDevice::CreateMainRenderTargetView()
	{
		ID3D11Texture2D* pBackBuffer;
		m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pMainRenderTargetView);
		DV_RELEASE(pBackBuffer);
	}

	void GraphicsDevice::CleanupMainRenderTargetView()
	{
		DV_RELEASE(m_pMainRenderTargetView);
	}

	void GraphicsDevice::ResizeBackBuffer(unsigned int width, unsigned int height)
	{
		DV_ASSERT(m_pSwapChain != nullptr);
		DV_ASSERT(m_pImmediateContext != nullptr);

		m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

		CleanupMainRenderTargetView();

		if (FAILED(m_pSwapChain->ResizeBuffers(m_BackbufferCount, width, height, m_Format, 0)))
		{
			DV_CORE_WARN("ResizeBuffer를 실패하였습니다.");
			return;
		}

		CreateMainRenderTargetView();

		m_Width = width;
		m_Height = height;
	}

	bool GraphicsDevice::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer* pBuffer)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc || !pData)
			return false;

		if (FAILED(m_pDevice->CreateBuffer(pDesc, pData, &pBuffer)))
			return false;
	
		return true;
	}

	bool GraphicsDevice::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Texture2D** ppTexture2D)
	{
		if (!m_pDevice)
			return false;

		if (!pDesc)
			return false;

		if (FAILED(m_pDevice->CreateTexture2D(pDesc, pData, ppTexture2D)))
			return false;
	
		return true;
	}
	
	bool GraphicsDevice::CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppShaderResourceView)
	{
		if (!m_pDevice)
			return false;

		if (!pResource || !pDesc)
			return false;

		if (FAILED(m_pDevice->CreateShaderResourceView(pResource, pDesc, ppShaderResourceView)))
			return false;

		return true;
	}
	
	bool GraphicsDevice::CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRenderTargetView)
	{
		if (!m_pDevice)
			return false;

		if (!pResource || !pDesc)
			return false;

		if (FAILED(m_pDevice->CreateRenderTargetView(pResource, pDesc, ppRenderTargetView)))
			return false;

		return true;
	}
	
	bool GraphicsDevice::CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView)
	{
		if (!m_pDevice)
			return false;

		if (!pResource || !pDesc)
			return false;

		if (FAILED(m_pDevice->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView)))
			return false;

		return true;
	}
}