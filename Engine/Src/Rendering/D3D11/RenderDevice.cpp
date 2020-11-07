#include "DivePch.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"

namespace Dive
{
	RenderDevice::RenderDevice()
	{
	}

	RenderDevice::~RenderDevice()
	{
	}
	
	bool RenderDevice::Initialize(HWND hwnd, int width, int height, bool vsync, bool windowed)
	{
		m_bVSync	= vsync;
		m_bWindowed = windowed;

		if (!IsWindow(hwnd))
		{
			CORE_ERROR("RenderDevice::Initialize >> ��ȿ���� ���� ������ �ڵ��Դϴ�.");
			return false;
		}

		//= SwapChain, Device, DeviceContext =======================================================================
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			swapChainDesc.BufferCount							= m_swap_chain_buffer_count;
			swapChainDesc.BufferDesc.Format						= static_cast<DXGI_FORMAT>(m_format);
			swapChainDesc.BufferDesc.Width						= width;
			swapChainDesc.BufferDesc.Height						= height;
			swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
			swapChainDesc.BufferDesc.RefreshRate.Numerator		= m_RefreshRate.x;
			swapChainDesc.BufferDesc.RefreshRate.Denominator	= m_RefreshRate.y;
			swapChainDesc.Windowed								= m_bWindowed ? TRUE : FALSE;
			swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.OutputWindow							= hwnd;
			swapChainDesc.SampleDesc.Count						= 1;
			swapChainDesc.SampleDesc.Quality					= 0;
			// �ϴ� �Ʒ� �� �׸��� �����ϸ� ������ �����Ѵ�.
			swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;// DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.Flags									= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			UINT deviceFlags;
#ifdef DEBUG
			deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#else
			deviceFlags = 0;
#endif
			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_1,
			};

			// ���� �̰� ������ ������ �߻��� �� ����.
			auto swap_chain_ptr			= m_SwapChain.get();
			auto device_ptr				= m_D3dDevice.get();
			auto immediate_context_ptr	= m_ImmediateContext.get();

			if (FAILED(D3D11CreateDeviceAndSwapChain(
				nullptr,					// ù ��° Adapter�� ����մϴ�.
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				deviceFlags,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				&swapChainDesc,
				&swap_chain_ptr,
				&device_ptr,
				nullptr,
				&immediate_context_ptr)))
			{
				CORE_ERROR("RenderDevice::Initialize >> D3D11 Device, DeviceContext, SwapChain ������ �����Ͽ����ϴ�.");
				return false;
			}
		}

		//= RenderTargetView =======================================================================================
		if (!createBackBufferRenderTarget())
			return false;

		m_Resolution = DirectX::XMINT2(width, height);

		CORE_TRACE("RenderDevice::RenderDevice >> RenderDevice �ʱ�ȭ�� �����Ͽ����ϴ�.");

		return true;
	}

	void RenderDevice::Present()
	{
		if (!m_SwapChain)
			return;


		// test rendering
		m_SwapChain->Present(0, 0);
	}

	void RenderDevice::ResizeResolution(unsigned int width, unsigned int height)
	{
		if (m_Resolution.x == width && m_Resolution.y == height)
			return;

		// ResizeTarget();
	}

	bool RenderDevice::createBackBufferRenderTarget()
	{
		if (!m_SwapChain)
			return false;

		ID3D11Texture2D* back_buffer_ptr = nullptr;
		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&back_buffer_ptr))))
		{
			CORE_ERROR("RenderDevice::createBackBufferRenderTarget>> Backbuffer ȹ�濡 �����Ͽ����ϴ�.");
			return false;
		}

		if (FAILED(m_D3dDevice->CreateRenderTargetView(back_buffer_ptr, nullptr, &m_BackBuffer)))
		{
			CORE_ERROR("RenderDevice::createBackBufferRenderTarget>> RenderTargetView ������ �����Ͽ����ϴ�.");
			return false;
		}

		SAFE_RELEASE(back_buffer_ptr);

		return true;
	}
}
