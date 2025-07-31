#include "stdafx.h"
#include "Graphics.h"
#include "core/Window.h"
#include "core/EventDispatcher.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	constexpr UINT DV_BUFFER_COUNT = 2;
	constexpr UINT DV_REFRESHRATE_NUMERATOR = 60;
	constexpr UINT DV_REFRESHRATE_DENOMINATOR = 1;
	constexpr DXGI_FORMAT DV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr BOOL DV_WINDOWED = TRUE;

	uint32_t Graphics::s_width = 0;
	uint32_t Graphics::s_height = 0;
	bool Graphics::m_useSync = false;

	ComPtr<IDXGISwapChain> Graphics::s_swapChain;
	ComPtr<ID3D11Device> Graphics::s_device;
	ComPtr<ID3D11DeviceContext> Graphics::s_deviceContext;

	ComPtr<ID3D11RenderTargetView> Graphics::s_renderTargetView;
	ComPtr<ID3D11Texture2D> Graphics::s_depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> Graphics::s_depthStencilView;

	bool Graphics::Initialize()
	{

		HRESULT hr = ::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			s_device.GetAddressOf(),
			nullptr,
			s_deviceContext.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::Initialize] D3D11CreateDevice 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		IDXGIDevice* dxgiDevice{};
		s_device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter{};
		dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
		IDXGIFactory* dxgiFactory{};
		dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferCount = DV_BUFFER_COUNT;
		desc.BufferDesc.Width = static_cast<UINT>(Window::GetWidth());
		desc.BufferDesc.Height = static_cast<UINT>(Window::GetHeight());
		desc.BufferDesc.Format = DV_FORMAT;
		desc.BufferDesc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.BufferDesc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !Window::IsFullScreen();
		desc.OutputWindow = Window::GetWindowHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		hr = dxgiFactory->CreateSwapChain(s_device.Get(), &desc, s_swapChain.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::Initialize] CreateSwapChain 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		DV_RELEASE(dxgiFactory);
		DV_RELEASE(dxgiAdapter);
		DV_RELEASE(dxgiDevice);

		SetupViews();

		DV_SUBSCRIBE_EVENT(eEventType::WindowResized, DV_EVENT_HANDLER_STATIC(OnResizeViews));

		DV_LOG(Graphics, info, "초기화 성공");

		return true;
	}
	
	void Graphics::Shutdown()
	{
		s_depthStencilView.Reset();
		s_depthStencilBuffer.Reset();
		s_renderTargetView.Reset();
		
		s_deviceContext.Reset();
		s_device.Reset();
		s_swapChain.Reset();

		DV_LOG(Graphics, info, "셧다운 완료");
	}

	// 윈도우 모드에서는 크기,
	// 전체화면 모드에서는 해상도를 갱신
	// 후 wm_size 발생
	void Graphics::Resize(uint32_t  width, uint32_t height)
	{
		assert(s_swapChain);

		DXGI_MODE_DESC desc{};
		desc.Width = static_cast<UINT>(width);
		desc.Height = static_cast<UINT>(height);
		desc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.Format = DV_FORMAT;

		HRESULT hr = s_swapChain->ResizeTarget(&desc);
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::Resize] ResizeTarget 실패: {}", ErrorUtils::ToVerbose(hr));
			return;
		}
	}

	void Graphics::OnResizeViews()
	{
		if (!ResizeSwapChain())
		{
			DV_LOG(Graphics, err, "[::OnResizeViews] 스왑체인 리사이즈 실패");
			return;
		}

		if(!SetupViews())
		{
			DV_LOG(Graphics, err, "[::OnResizeViews] 뷰 재생성 실패");
			return;
		}
	}

	void Graphics::Present()
	{
		assert(s_swapChain);
		s_swapChain->Present(m_useSync ? 1 : 0, 0);
	}

	IDXGISwapChain* Graphics::GetSwapChain()
	{
		return s_swapChain.Get();
	}
	
	ID3D11Device* Graphics::GetDevice()
	{
		assert(s_device);
		return s_device.Get();
	}
	
	ID3D11DeviceContext* Graphics::GetDeviceContext()
	{
		assert(s_deviceContext);
		return s_deviceContext.Get();
	}

	ID3D11RenderTargetView* Graphics::GetRenderTargetView()
	{
		assert(s_renderTargetView);
		return s_renderTargetView.Get();
	}
	
	ID3D11DepthStencilView* Graphics::GetDepthStencilView()
	{
		assert(s_depthStencilView);
		return s_depthStencilView.Get();
	}

	bool Graphics::SetupViews()
	{
		assert(s_swapChain.Get());
		assert(s_device.Get());

		s_renderTargetView.Reset();
		s_depthStencilBuffer.Reset();
		s_depthStencilView.Reset();
		
		ID3D11Texture2D* backBuffer = nullptr;
		HRESULT hr = s_swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBuffer);
		if(FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::SetViews] GetBuffer 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		hr = s_device->CreateRenderTargetView(static_cast<ID3D11Resource*>(backBuffer), nullptr, s_renderTargetView.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::SetViews] CreateRenderTargetView 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}
		// 반드시 릴리즈!!!
		backBuffer->Release();
		
		DXGI_SWAP_CHAIN_DESC desc{};
		s_swapChain->GetDesc(&desc);

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = desc.BufferDesc.Width;
		texDesc.Height = desc.BufferDesc.Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		hr = s_device->CreateTexture2D(&texDesc, nullptr, s_depthStencilBuffer.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::SetViews] CreateTexture2D 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		hr = s_device->CreateDepthStencilView(static_cast<ID3D11Resource*>(s_depthStencilBuffer.Get()), &viewDesc, s_depthStencilView.GetAddressOf());
		if(FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::SetViews] CreateDepthStencilView 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		s_width = static_cast<uint32_t>(desc.BufferDesc.Width);
		s_height = static_cast<uint32_t>(desc.BufferDesc.Height);

		return true;
	}

	bool Graphics::ResizeSwapChain()
	{
		assert(s_swapChain);
		assert(s_deviceContext);

		s_deviceContext->OMSetRenderTargets(0, NULL, NULL);
		s_renderTargetView.Reset();

		HRESULT hr = s_swapChain->ResizeBuffers(0, 0, 0, DV_FORMAT, 0);
		if (FAILED(hr))
		{
			DV_LOG(Graphics, err, "[::ResizeSwapChain] ResizeBuffers 실패: {}", ErrorUtils::ToVerbose(hr));
			return false;
		}

		return true;
	}
}
