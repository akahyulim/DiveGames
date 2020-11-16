#include "DivePch.h"
#include "SwapChain.h"
#include "Core/Log.h"
#include "RenderDevice.h"

namespace Dive
{
	// �ϴ� �ִ��� �ܼ��ϰ� ����
	// ���� windowed, buffer count, target frame rate, format ���� �޵��� ������ �ʿ���.
	SwapChain::SwapChain(const std::shared_ptr<RenderDevice>& device, HWND hWnd, int width, int height)
	{
		m_RenderDevice	= device;
		m_hWnd			= hWnd;
		m_Width			= width;
		m_Height		= height;

		// factory �ʿ�
		// => device�κ��� ������.

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

		// �ϴ� ���� ȹ��
		IDXGIFactory* factory;
		if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
		{
			CORE_ERROR("Factory ������ �����Ͽ����ϴ�.");
			return;
		}

		if (FAILED(factory->CreateSwapChain(m_RenderDevice->GetD3DDevice(), &swapChainDesc, &m_SwapChain)))
		{
			CORE_ERROR("SwapChain ������ �����Ͽ����ϴ�.");
			return;
		}

		// render target view ����
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

		// ��� �� �Լ��� 2, 3��° ���ڸ� 0���� �����ϸ� hWnd�� ũ�⸦ �ڵ����� �����´�.
		// ���� ���� �Ű������� ���޹��� �ʿ䰡 ����.
		// ũ�� Ȯ�� ���� desc�� ������ �� �� ����.
		m_Width		= width;
		m_Height	= height;
		CORE_TRACE("Resize Resolution Width: {0:d}, Height: {1:d}", m_Width, m_Height);

		// render target view�� ���� �����ؾ����� ������?
	}
}
