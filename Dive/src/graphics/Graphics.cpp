#include "stdafx.h"
#include "Graphics.h"
#include "core/CoreDefs.h"
#include "core/Window.h"

namespace Dive
{
	constexpr uint32_t DV_BUFFER_COUNT = 2;
	constexpr uint32_t DV_REFRESHRATE_NUMERATOR = 60;
	constexpr uint32_t DV_REFRESHRATE_DENOMINATOR = 1;
	constexpr DXGI_FORMAT DV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr BOOL DV_WINDOWED = TRUE;

	IDXGISwapChain* Graphics::s_SwapChain = nullptr;
	ID3D11Device* Graphics::s_Device = nullptr;
	ID3D11DeviceContext* Graphics::s_DeviceContext = nullptr;

	ID3D11RenderTargetView* Graphics::s_RenderTargetView = nullptr;
	ID3D11Texture2D* Graphics::s_DepthStencilTexture = nullptr;
	ID3D11DepthStencilView* Graphics::s_DEpthStencilView = nullptr;

	uint32_t Graphics::s_ResolutionWidth = 0;
	uint32_t Graphics::s_ResolutionHeight = 0;
	bool Graphics::s_UseVSync = false;

	void Graphics::Initialize()
	{
		if (FAILED(::D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&s_Device,
			nullptr,
			&s_DeviceContext))) 
		{
			Shutdown();
			DV_LOG(Graphics, err, "D3D11Device & D3D11DeviceContext 생성에 실패하였습니다.");
			return;
		}

		IDXGIDevice* dxgiDevice{};
		s_Device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter{};
		dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
		IDXGIFactory* dxgiFactory{};
		dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferCount = DV_BUFFER_COUNT;
		desc.BufferDesc.Width = Window::GetWidth();
		desc.BufferDesc.Height = Window::GetHeight();
		desc.BufferDesc.Format = DV_FORMAT;
		desc.BufferDesc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.BufferDesc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// 멀티 샘플링 off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = !Window::IsFullScreen();
		desc.OutputWindow = Window::GetWindowHandle();
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek에선 0이고 다른 값들 설정이 남아 있다...

		if (FAILED(dxgiFactory->CreateSwapChain(s_Device, &desc, &s_SwapChain)))
		{
			Shutdown();
			DV_LOG(Graphics, err, "D3D11SwapChain 생성에 실패하였습니다.");
			return;
		}

		DV_RELEASE(dxgiFactory);
		DV_RELEASE(dxgiAdapter);
		DV_RELEASE(dxgiDevice);

		updateBackbuffer();

		DV_SUBSCRIBE_EVENT(eEventType::WindowResized, DV_EVENT_HANDLER_DATA_STATIC(OnWindowResized));
	}
	
	void Graphics::Shutdown()
	{
		DV_RELEASE(s_DEpthStencilView);
		DV_RELEASE(s_DepthStencilTexture);
		DV_RELEASE(s_RenderTargetView);
		
		DV_RELEASE(s_DeviceContext);
		DV_RELEASE(s_Device);
		DV_RELEASE(s_SwapChain);
	}

	void Graphics::ChangeResolution(uint32_t width, uint32_t height)
	{
		DV_ASSERT(Graphics, s_SwapChain);

		if (s_ResolutionWidth == width && s_ResolutionHeight == height) 
			return;

		Window::Hide();

		s_ResolutionWidth = width; 
		s_ResolutionHeight = height;

		DV_RELEASE(s_RenderTargetView);
	
		if (FAILED(s_SwapChain->ResizeBuffers(DV_BUFFER_COUNT, width, height, DV_FORMAT, 0))) 
		{
			DV_LOG(Graphics, err, "후면 버퍼 크기 {0:d}x{1:d} 변경에 실패하였습니다.", width, height);
			return;
		}

		DXGI_MODE_DESC desc{};
		desc.Width = width;
		desc.Height = height;
		desc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.Format = DV_FORMAT;

		if (FAILED(s_SwapChain->ResizeTarget(&desc))) 
		{
			DV_LOG(Graphics, err, "해상도 {0:d}x{1:d} 변경에 실패하였습니다.", width, height);
			return;
		}

		updateBackbuffer();

		Window::Show();
	}

	void Graphics::OnWindowResized(EventData data)
	{
		if (auto pairData = std::get_if<void*>(&data)) 
		{
			auto actualData = static_cast<std::pair<uint32_t, uint32_t>*>(*pairData);
			ChangeResolution(actualData->first, actualData->second);
		}
	}

	void Graphics::Present()
	{
		DV_ASSERT(Graphics, s_SwapChain);
		s_SwapChain->Present(s_UseVSync ? 1 : 0, 0);
	}

	IDXGISwapChain* Graphics::GetSwapChain()
	{
		DV_ASSERT(Graphics, s_SwapChain);
		return s_SwapChain;
	}
	
	ID3D11Device* Graphics::GetDevice()
	{
		DV_ASSERT(Graphics, s_Device);
		return s_Device;
	}
	
	ID3D11DeviceContext* Graphics::GetDeviceContext()
	{
		DV_ASSERT(Graphics, s_DeviceContext);
		return s_DeviceContext;
	}

	ID3D11RenderTargetView* Graphics::GetBackbufferRTV()
	{
		DV_ASSERT(Graphics, s_RenderTargetView);
		return s_RenderTargetView;
	}
	
	ID3D11DepthStencilView* Graphics::GetBackbufferDSV()
	{
		DV_ASSERT(Graphics, s_DEpthStencilView);
		return s_DEpthStencilView;
	}

	void Graphics::updateBackbuffer()
	{
		DV_ASSERT(Graphics, s_SwapChain);
		DV_ASSERT(Graphics, s_Device);

		DV_RELEASE(s_RenderTargetView);
		DV_RELEASE(s_DepthStencilTexture);
		DV_RELEASE(s_DEpthStencilView);

		ID3D11Texture2D* pBackbufferTexture{};
		if (FAILED(s_SwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 텍스쳐 획득 실패");
			return;
		}

		if (FAILED(s_Device->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), 
			nullptr, 
			&s_RenderTargetView))) 
		{
			DV_RELEASE(s_RenderTargetView);
			DV_LOG(Graphics, err, "후면버퍼 렌더타겟뷰 생성 실패");
			return;
		}
		DV_RELEASE(pBackbufferTexture);

		DXGI_SWAP_CHAIN_DESC desc{};
		s_SwapChain->GetDesc(&desc);

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = desc.BufferDesc.Width;
		texDesc.Height = desc.BufferDesc.Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT32>(screenParamm_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParamm_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(s_Device->CreateTexture2D(&texDesc, nullptr, &s_DepthStencilTexture))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 깊이 스텐실 텍스쳐 생성 실패");
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(s_Device->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(s_DepthStencilTexture),
			&viewDesc,
			&s_DEpthStencilView))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 깊이 스텐실 뷰 생성 실패");
			return;
		}

		s_ResolutionWidth = desc.BufferDesc.Width;
		s_ResolutionHeight = desc.BufferDesc.Height;
	}
}