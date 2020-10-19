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
	
	bool RenderDevice::Initialize(HWND hwnd, unsigned int width, unsigned int height)
	{
		if (!IsWindow(hwnd))
		{
			CORE_ERROR("RenderDevice::Initialize >> 유효하지 않은 윈도우 핸들입니다.");
			return false;
		}

		//= SwapChain, Device, DeviceContext =======================================================================
		{
			DXGI_SWAP_CHAIN_DESC desc_swap_chain;
			ZeroMemory(&desc_swap_chain, sizeof(desc_swap_chain));
			desc_swap_chain.BufferCount							= m_swap_chain_buffer_count;
			desc_swap_chain.BufferDesc.Format					= static_cast<DXGI_FORMAT>(m_format);
			desc_swap_chain.BufferDesc.Width					= width;
			desc_swap_chain.BufferDesc.Height					= height;
			desc_swap_chain.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			desc_swap_chain.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
			desc_swap_chain.BufferDesc.RefreshRate.Numerator	= m_refresh_rate.x;
			desc_swap_chain.BufferDesc.RefreshRate.Denominator	= m_refresh_rate.y;
			desc_swap_chain.Windowed							= m_is_windowed ? TRUE : FALSE;
			desc_swap_chain.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc_swap_chain.OutputWindow						= hwnd;
			desc_swap_chain.SampleDesc.Count					= 1;
			desc_swap_chain.SampleDesc.Quality					= 0;
			// 일단 아래 두 항목을 수정하면 생성에 실패한다.
			desc_swap_chain.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;// DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc_swap_chain.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			UINT device_flags;
#ifdef DEBUG
			device_flags = D3D11_CREATE_DEVICE_DEBUG;
#else
			device_flags = 0;
#endif
			D3D_FEATURE_LEVEL feature_levels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3,
				D3D_FEATURE_LEVEL_9_1,
			};

			auto swap_chain_ptr			= m_swap_chain.get();
			auto device_ptr				= m_device.get();
			auto immediate_context_ptr	= m_immediate_context.get();

			if (FAILED(D3D11CreateDeviceAndSwapChain(
				nullptr,					// 첫 번째 Adapter를 사용합니다.
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				device_flags,
				feature_levels,
				ARRAYSIZE(feature_levels),
				D3D11_SDK_VERSION,
				&desc_swap_chain,
				&swap_chain_ptr,
				&device_ptr,
				nullptr,
				&immediate_context_ptr)))
			{
				CORE_ERROR("RenderDevice::Initialize >> D3D11 Device, DeviceContext, SwapChain 생성에 실패하였습니다.");
				return false;
			}
		}

		//= RenderTargetView =======================================================================================
		if (!createBackBufferRenderTarget())
			return false;

		m_resolution = DirectX::XMINT2(width, height);

		CORE_TRACE("RenderDevice::RenderDevice >> RenderDevice 초기화에 성공하였습니다.");

		return true;
	}

	void RenderDevice::Present()
	{
		if (!m_swap_chain)
			return;


		// test rendering
		m_swap_chain->Present(0, 0);
	}

	bool RenderDevice::createBackBufferRenderTarget()
	{
		if (!m_swap_chain)
			return false;

		ID3D11Texture2D* back_buffer_ptr = nullptr;
		if (FAILED(m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer_ptr))))
		{
			CORE_ERROR("RenderDevice::createBackBufferRenderTarget>> Backbuffer 획득에 실패하였습니다.");
			return false;
		}

		if (FAILED(m_device->CreateRenderTargetView(back_buffer_ptr, nullptr, &m_back_buffer_RTV)))
		{
			CORE_ERROR("RenderDevice::createBackBufferRenderTarget>> RenderTargetView 생성에 실패하였습니다.");
			return false;
		}

		SAFE_RELEASE(back_buffer_ptr);

		return true;
	}
}
