#include "DivePch.h"
#include "SwapChain.h"
#include "Core/Log.h"

namespace Dive
{
	// ������ �ڵ�, ũ��, â ��� ����, vsync, frame rate, format, buffer count
	SwapChain::SwapChain()
	{
		// swapchain�� IDXGIFactory -> RenderDevice�κ��� ���´�.
		// RenderTargetView�� D3DDevice�� �����.

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
