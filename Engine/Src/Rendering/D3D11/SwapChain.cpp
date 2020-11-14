#include "DivePch.h"
#include "SwapChain.h"
#include "Core/Log.h"

namespace Dive
{
	// 윈도우 핸들, 크기, 창 모드 여부, vsync, frame rate, format, buffer count
	SwapChain::SwapChain()
	{
		// swapchain은 IDXGIFactory -> RenderDevice로부터 얻어온다.
		// RenderTargetView는 D3DDevice로 만든다.

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	}

	SwapChain::~SwapChain()
	{
	}

	void SwapChain::ResizeResolution(int width, int height)
	{
		m_Width = width;
		m_Height = height;

		CORE_TRACE("Resize Resolution Width: {0:d}, Height: {1:d}", m_Width, m_Height);
	}
}
