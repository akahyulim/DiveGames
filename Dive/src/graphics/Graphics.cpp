#include "stdafx.h"
#include "Graphics.h"
#include "core/CoreDefs.h"
#include "core/Window.h"
#include "RenderTexture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

using Microsoft::WRL::ComPtr;

namespace Dive
{
	constexpr uint32_t DV_BUFFER_COUNT = 2;
	constexpr uint32_t DV_REFRESHRATE_NUMERATOR = 60;
	constexpr uint32_t DV_REFRESHRATE_DENOMINATOR = 1;
	constexpr DXGI_FORMAT DV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr BOOL DV_WINDOWED = TRUE;

	uint32_t Graphics::s_ResolutionWidth = 0;
	uint32_t Graphics::s_ResolutionHeight = 0;
	bool Graphics::s_UseVSync = false;

	ComPtr<IDXGISwapChain> Graphics::s_SwapChain;
	ComPtr<ID3D11Device> Graphics::s_Device;
	ComPtr<ID3D11DeviceContext> Graphics::s_DeviceContext;

	ComPtr<ID3D11RenderTargetView> Graphics::s_BackBufferRTV;
	ComPtr<ID3D11Texture2D> Graphics::s_BackbufferTexture;
	ComPtr<ID3D11DepthStencilView> Graphics::s_BackBufferDSV;

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
			s_Device.GetAddressOf(),
			nullptr,
			s_DeviceContext.GetAddressOf()))) 
		{
			Shutdown();
			DV_LOG(Graphics, critical, "D3D11Device & D3D11DeviceContext 생성 실패!.");
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

		if (FAILED(dxgiFactory->CreateSwapChain(s_Device.Get(), &desc, s_SwapChain.GetAddressOf())))
		{
			DV_LOG(Graphics, critical, "D3D11SwapChain 생성 실패!");
			return;
		}

		DV_RELEASE(dxgiFactory);
		DV_RELEASE(dxgiAdapter);
		DV_RELEASE(dxgiDevice);

		updateBackbuffer();

		DV_SUBSCRIBE_EVENT(eEventType::WindowResized, DV_EVENT_HANDLER_DATA_STATIC(OnWindowResized));

		DV_LOG(Graphics, info, "초기화 완료");
	}
	
	void Graphics::Shutdown()
	{
		s_BackBufferDSV.Reset();
		s_BackbufferTexture.Reset();
		s_BackBufferRTV.Reset();
		
		s_DeviceContext.Reset();
		s_Device.Reset();
		s_SwapChain.Reset();

		DV_LOG(Graphics, info, "셧다운 완료");
	}

	void Graphics::ChangeResolution(uint32_t width, uint32_t height)
	{
		assert(s_SwapChain);

		if (s_ResolutionWidth == width && s_ResolutionHeight == height) 
			return;

		Window::Hide();

		s_ResolutionWidth = width; 
		s_ResolutionHeight = height;
	
		if (FAILED(s_SwapChain->ResizeBuffers(DV_BUFFER_COUNT, static_cast<UINT>(width), static_cast<UINT>(height), DV_FORMAT, 0)))
		{
			DV_LOG(Graphics, err, "후면 버퍼 크기 {0:d}x{1:d} 변경 실패", width, height);
			return;
		}

		DXGI_MODE_DESC desc{};
		desc.Width = static_cast<UINT>(width);
		desc.Height = static_cast<UINT>(height);
		desc.RefreshRate.Numerator = DV_REFRESHRATE_NUMERATOR;
		desc.RefreshRate.Denominator = DV_REFRESHRATE_DENOMINATOR;
		desc.Format = DV_FORMAT;

		if (FAILED(s_SwapChain->ResizeTarget(&desc))) 
		{
			DV_LOG(Graphics, err, "해상도 {0:d}x{1:d} 변경 실패", width, height);
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
		assert(s_SwapChain.Get());
		s_SwapChain->Present(s_UseVSync ? 1 : 0, 0);
	}

	IDXGISwapChain* Graphics::GetSwapChain()
	{
		return s_SwapChain.Get();
	}
	
	ID3D11Device* Graphics::GetDevice()
	{
		return s_Device.Get();
	}
	
	ID3D11DeviceContext* Graphics::GetDeviceContext()
	{
		return s_DeviceContext.Get();
	}

	ID3D11RenderTargetView* Graphics::GetBackBufferRTV()
	{
		return s_BackBufferRTV.Get();
	}
	
	ID3D11DepthStencilView* Graphics::GetBackBufferDSV()
	{
		return s_BackBufferDSV.Get();
	}

	void Graphics::updateBackbuffer()
	{
		assert(s_SwapChain.Get());
		assert(s_Device.Get());

		s_BackBufferRTV.Reset();
		s_BackbufferTexture.Reset();
		s_BackBufferDSV.Reset();
		
		ID3D11Texture2D* backBufferTexture{};
		if (FAILED(s_SwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backBufferTexture))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 텍스쳐 획득 실패");
			return;
		}

		if (FAILED(s_Device->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(backBufferTexture), 
			nullptr, 
			&s_BackBufferRTV))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 렌더타겟뷰 생성 실패");
			return;
		}
		
		DXGI_SWAP_CHAIN_DESC desc{};
		s_SwapChain->GetDesc(&desc);

		D3D11_TEXTURE2D_DESC texDesc{};
		texDesc.Width = desc.BufferDesc.Width;
		texDesc.Height = desc.BufferDesc.Height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<uint32_t>(screenParamm_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParamm_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(s_Device->CreateTexture2D(&texDesc, nullptr, &s_BackbufferTexture))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 깊이 스텐실 텍스쳐 생성 실패");
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(s_Device->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(s_BackbufferTexture.Get()),
			&viewDesc,
			s_BackBufferDSV.GetAddressOf()))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 깊이 스텐실 뷰 생성 실패");
			return;
		}

		s_ResolutionWidth = static_cast<uint32_t>(desc.BufferDesc.Width);
		s_ResolutionHeight = static_cast<uint32_t>(desc.BufferDesc.Height);
	}
}
