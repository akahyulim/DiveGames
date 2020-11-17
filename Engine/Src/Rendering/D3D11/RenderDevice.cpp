#include "DivePch.h"
#include "RenderDevice.h"
#include "Core/Log.h"
#include "Core/DiveDefs.h"

namespace Dive
{
	// 일단 default adapter로 구현
	RenderDevice::RenderDevice(HWND hWnd, int width, int height, bool windowed)
		: m_hWnd(hWnd), m_Width(width), m_Height(height), m_bWindowed(windowed)
	{
		UINT deviceFlags = 0;
#ifdef _DEBUG
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
		desc.OutputWindow							= m_hWnd;
		desc.BufferDesc.Width						= m_Width;
		desc.BufferDesc.Height						= m_Height;
		desc.Windowed								= m_bWindowed;
		desc.BufferCount							= 1;							// 매개변수 대상
		desc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;	// 매개변수 대상
		desc.BufferDesc.RefreshRate.Denominator		= 1;							// 매개변수 대상
		desc.BufferDesc.RefreshRate.Numerator		= 0;							// 매개변수 대상
		desc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling						= DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count						= 1;
		desc.SampleDesc.Quality						= 0;
		desc.SwapEffect								= DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags									= 0;

		auto createDevice = [this, deviceFlags, featureLevels, driverType, desc]()
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
				&m_SwapChain,
				&m_D3dDevice,
				nullptr,
				&m_ImmediateContext);
		};

		auto result = createDevice();
		if (result == DXGI_ERROR_SDK_COMPONENT_MISSING)
		{
			CORE_WARN("");
			deviceFlags &= ~D3D11_CREATE_DEVICE_DEBUG;
			result = createDevice();
		}

		if (FAILED(result))
		{
			CORE_ERROR("");
			return;
		}

		// render target view 생성
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return;
		}

		result = m_D3dDevice->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
		backBuffer->Release();
		if (FAILED(result))
		{
			CORE_ERROR("");
			return;
		}

		m_bInitialize = true;
	}
	
	RenderDevice::~RenderDevice()
	{
		if (m_ImmediateContext)
		{
			m_ImmediateContext->Release();
			m_ImmediateContext = nullptr;
		}

		if (m_D3dDevice)
		{
			m_D3dDevice->Release();
			m_D3dDevice = nullptr;
		}
	}

	bool RenderDevice::Present()
	{
		if (!m_SwapChain)
		{
			CORE_ERROR("");
			return false;
		}

		if (FAILED(m_SwapChain->Present(0, 0)))
		{
			CORE_ERROR("");
			return false;
		}

		return true;
	}

	void RenderDevice::ResizeBuffer(int width, int height)
	{
		if (width == 0 || height == 0)
		{
			CORE_ERROR("");
			return;
		}

		// 예상과 다른 점
		// 1. width, height을 0으로 넣어도 desc의 값 변경이 없다.
		// 2. width, height를 전달받은 값으로 넣어도 desc의 값 변경이 없다.
		// 즉, 버퍼의 크기가 변경되지 않았다.
		m_SwapChain->ResizeBuffers(
			1,
			width,
			height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0);

		m_Width = width;
		m_Height = height;
		CORE_TRACE("Resize Resolution Width: {0:d}, Height: {1:d}", m_Width, m_Height);

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		m_SwapChain->GetDesc(&desc);
		CORE_TRACE("desc Resolution Width: {0:d}, Height: {1:d}", desc.BufferDesc.Width, desc.BufferDesc.Height);

		// back buffer 크기가 변경되지 않으니 직접 가져와도 크기의 변화가 없다.
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return;
		}
		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		backBuffer->GetDesc(&texDesc);
		backBuffer->Release();
		CORE_TRACE("tex Resolution Width: {0:d}, Height: {1:d}", texDesc.Width, texDesc.Height);


		// render target view는 새로 생성해야하지 않을까?
	}
}
