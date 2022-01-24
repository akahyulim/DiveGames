#include "divepch.h"
#include "GraphicsDevice.h"
#include "../../Helper/Log.h"
#include "../../Core/DiveCore.h"

using namespace Microsoft::WRL;


// 1. �����ڿ� ���� �� �� �����غ� �� �����ؾ� �Ѵ�.
// 2. ���� Frame Rate�� �������� �ʰ� �ִ�.
// Frame Rate ����ÿ��� ResizeTarget()�� ����ϴ� ���ϱ�?
namespace Dive
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
				nullptr,									// ���� �ӽ�
				driverType,
				nullptr,
				deviceFlags,
				featureLevels.data(),
				static_cast<UINT>(featureLevels.size()),
				D3D11_SDK_VERSION,
				&desc,
				m_pSwapChain.GetAddressOf(),
				m_pDevice.GetAddressOf(),
				nullptr,
				m_pImmediateContext.GetAddressOf());
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
			CORE_ERROR("Graphics Device ������ �����Ͽ����ϴ�.");
		}
	}

	void GraphicsDevice::PresentBegin(float* pClearColor)
	{
		m_pImmediateContext->OMSetRenderTargets(1, m_pBackbufferRTV.GetAddressOf(), 0);
		
		float clearColors[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_pImmediateContext->ClearRenderTargetView(m_pBackbufferRTV.Get(), pClearColor ? pClearColor : clearColors);
	}
	
	void GraphicsDevice::PresentEnd()
	{
		m_pSwapChain->Present(static_cast<UINT>(m_bVSync), 0);
	}

	// ������ ũ�� Ȥ�� �ػ󵵸� ������ �� ����Ѵ�.
	// ResizeResolution���� �̸��� �ٲ���.
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

	// WM_SIZE�� �����Ͽ� Backbuffer ũ�⸦ �����Ѵ�.
	// �� �Լ��� �̿��Ͽ� ������, �ػ� ������ �Ұ����ϴ�.
	// ResizeBackbuffer�� �̸��� �ٲ���.
	// ���� Editor�� Runtime���� �̺�Ʈ�� ó���ϸ鼭 �� �Լ��� ȣ���ϰ� �ִ�.
	void GraphicsDevice::SetResolution(UINT width, UINT height)
	{
		if ((width != m_ResolutionWidth || height != m_ResolutionHeight) && (width > 0 || height > 0))
		{
			m_ResolutionWidth = width;
			m_ResolutionHeight = height;

			m_pBackbuffer.Reset();
			m_pBackbufferRTV.Reset();

			if (FAILED(m_pSwapChain->ResizeBuffers(m_BackbufferCount, width, height, m_Format, 0)))
			{
				CORE_ERROR("GraphicsDevice::SetResolution>> SwapChain�� ReiszeBuffers�� �����Ͽ����ϴ�.");
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
		m_pBackbuffer.Reset();
		m_pBackbufferRTV.Reset();

		DV_ASSERT(m_pSwapChain != nullptr);

		if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)m_pBackbuffer.GetAddressOf())))
		{
			CORE_ERROR("GraphicsDevice::createBackbufferResource>> BackBuffer ������ �����Ͽ����ϴ�.");
		}

		DV_ASSERT(m_pDevice != nullptr);

		if (FAILED(m_pDevice->CreateRenderTargetView(m_pBackbuffer.Get(), nullptr, m_pBackbufferRTV.GetAddressOf())))
		{
			CORE_ERROR("GraphicsDevice::createBackbufferResource>> RenderTargetView ������ �����Ͽ����ϴ�.");
		}
	}
}