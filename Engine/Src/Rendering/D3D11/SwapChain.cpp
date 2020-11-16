#include "DivePch.h"
#include "SwapChain.h"
#include "Core/Log.h"
#include "RenderDevice.h"

namespace Dive
{
	// 일단 최대한 단순하게 구성
	// 추후 windowed, buffer count, target frame rate, format 등을 받도록 수정이 필요함.
	SwapChain::SwapChain(const std::shared_ptr<RenderDevice>& device, HWND hWnd, int width, int height)
	{
		m_RenderDevice	= device;
		m_hWnd			= hWnd;
		m_Width			= width;
		m_Height		= height;

		// factory 필요
		// => device로부터 얻어오기.

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount							= 1;
		swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.Width						= m_Width;
		swapChainDesc.BufferDesc.Height						= m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
		swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow							= m_hWnd;
		swapChainDesc.Windowed								= true;
		swapChainDesc.SampleDesc.Count						= 1;
		swapChainDesc.SampleDesc.Quality					= 0;
		swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags									= 0;

		// 일단 직접 획득
		IDXGIFactory* factory;
		if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		{
			CORE_ERROR("Factory 생성에 실패하였습니다.");
			return;
		}

		if (FAILED(factory->CreateSwapChain(m_RenderDevice->GetD3DDevice(), &swapChainDesc, &m_SwapChain)))
		{
			CORE_ERROR("SwapChain 생성에 실패하였습니다.");
			return;
		}

		// render target view 생성
		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
		{
			CORE_ERROR("");
			return;
		}

		auto result = m_RenderDevice->GetD3DDevice()->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
		backBuffer->Release();
		if(FAILED(result))
		{
			CORE_ERROR("");
			return;
		}
	}

	SwapChain::~SwapChain()
	{
	}

	bool SwapChain::Present()
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

	void SwapChain::ResizeBuffer(int width, int height)
	{
		if (width == 0 || height == 0)
		{
			CORE_ERROR("");
			return;
		}

		m_SwapChain->ResizeBuffers(
			1,
			0,
			0,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			0);

		// 사실 위 함수의 2, 3번째 인자를 0으로 전달하면 hWnd의 크기를 자동으로 가져온다.
		// 따라서 굳이 매개변수를 전달받을 필요가 없다.
		// 크기 확인 역시 desc를 얻어오면 될 것 같다.
		m_Width		= width;
		m_Height	= height;
		CORE_TRACE("Resize Resolution Width: {0:d}, Height: {1:d}", m_Width, m_Height);

		// render target view는 새로 생성해야하지 않을까?
	}
}
