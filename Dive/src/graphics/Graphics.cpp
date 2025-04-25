#include "stdafx.h"
#include "Graphics.h"
#include "core/CoreDefs.h"
#include "core/Window.h"
#include "RenderTexture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Dive
{
	constexpr UINT32 DV_BUFFER_COUNT = 2;
	constexpr UINT32 DV_REFRESHRATE_NUMERATOR = 60;
	constexpr UINT32 DV_REFRESHRATE_DENOMINATOR = 1;
	constexpr DXGI_FORMAT DV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr BOOL DV_WINDOWED = TRUE;

	UINT32 Graphics::s_ResolutionWidth = 0;
	UINT32 Graphics::s_ResolutionHeight = 0;
	bool Graphics::s_UseVSync = false;

	IDXGISwapChain* Graphics::s_SwapChain = nullptr;
	ID3D11Device* Graphics::s_Device = nullptr;
	ID3D11DeviceContext* Graphics::s_DeviceContext = nullptr;

	ID3D11RenderTargetView* Graphics::s_BackbufferRenderTarget = nullptr;
	ID3D11Texture2D* Graphics::s_BackbufferTexture = nullptr;
	ID3D11DepthStencilView* Graphics::s_BackbufferDepthStencil = nullptr;

	ID3D11DepthStencilView* Graphics::s_CurrentDepthStencil = nullptr;

	ID3D11DepthStencilState* Graphics::s_CurrentDepthStencilState = nullptr;
	ID3D11RasterizerState* Graphics::s_CurrentRasterizerState = nullptr;
	ID3D11BlendState* Graphics::s_CurrentBlendState = nullptr;

	D3D11_PRIMITIVE_TOPOLOGY Graphics::s_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	Shader* Graphics::s_CurrentVertexShader = nullptr;
	Shader* Graphics::s_CurrentHullShader = nullptr;
	Shader* Graphics::s_CurrentDomainShader = nullptr;
	Shader* Graphics::s_CurrentGeometryShader = nullptr;
	Shader* Graphics::s_CurrentPixelShader = nullptr;
	Shader* Graphics::s_CurrentComputeShader = nullptr;

	VertexBuffer* Graphics::s_CurrentVertexBuffer = nullptr;
	IndexBuffer* Graphics::s_CurrentIndexBuffer = nullptr;

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
		DV_RELEASE(s_BackbufferDepthStencil);
		DV_RELEASE(s_BackbufferTexture);
		DV_RELEASE(s_BackbufferRenderTarget);
		
		DV_RELEASE(s_DeviceContext);
		DV_RELEASE(s_Device);
		DV_RELEASE(s_SwapChain);
	}

	void Graphics::ChangeResolution(UINT32 width, UINT32 height)
	{
		DV_ASSERT(Graphics, s_SwapChain);

		if (s_ResolutionWidth == width && s_ResolutionHeight == height) 
			return;

		Window::Hide();

		s_ResolutionWidth = width; 
		s_ResolutionHeight = height;

		DV_RELEASE(s_BackbufferRenderTarget);
	
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
			auto actualData = static_cast<std::pair<UINT32, UINT32>*>(*pairData);
			ChangeResolution(actualData->first, actualData->second);
		}
	}

	void Graphics::Present()
	{
		DV_ASSERT(Graphics, s_SwapChain);
		s_SwapChain->Present(s_UseVSync ? 1 : 0, 0);
	}

	void Graphics::SetRenderTarget(ID3D11RenderTargetView* renderTargetView, ID3D11DepthStencilView* depthStencilView)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

	}

	void Graphics::ClearRenderTargetView(ID3D11RenderTargetView* renderTargetView, const DirectX::XMFLOAT4& color)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (renderTargetView)
		{
			FLOAT clearColor[4]{ color.x, color.y, color.z, color.w };
			s_DeviceContext->ClearRenderTargetView(renderTargetView, clearColor);
		}
	}

	void Graphics::ClearDepthStencilView(ID3D11DepthStencilView* depthStencilView, uint8_t flags, float depth, uint8_t stencil)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (depthStencilView)
			s_DeviceContext->ClearDepthStencilView(depthStencilView, flags, depth, stencil);
	}


	void Graphics::SetViewport(const RECT& rt)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = (float)rt.left;
		viewport.TopLeftY = (float)rt.top;
		viewport.Width = (float)(rt.right - rt.left);
		viewport.Height = (float)(rt.bottom - rt.top);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		s_DeviceContext->RSSetViewports(1, &viewport);
	}

	void Graphics::SetViewport(float width, float height, float x, float y)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = x;
		viewport.TopLeftY = y;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f; 
		viewport.MaxDepth = 1.0f;

		s_DeviceContext->RSSetViewports(1, &viewport);
	}

	void Graphics::SetDepthStencilState(ID3D11DepthStencilState* state)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (state != s_CurrentDepthStencilState)
		{
			s_DeviceContext->OMSetDepthStencilState(state, 0);
			s_CurrentDepthStencilState = state;
		}
	}

	void Graphics::SetRasterizerState(ID3D11RasterizerState* state)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (state != s_CurrentRasterizerState)
		{
			s_DeviceContext->RSSetState(state);
			s_CurrentRasterizerState = state;
		}
	}

	void Graphics::SetBlendState(ID3D11BlendState* state)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (state != s_CurrentBlendState)
		{
			float blendFactor[4]{};
			s_DeviceContext->OMSetBlendState(state, blendFactor, 0xffffffff);
			s_CurrentBlendState = state;
		}
	}

	void Graphics::SetVertexShader(Shader* shader)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentVertexShader == shader)
			return;

		if (shader)
		{
			DV_ASSERT(Graphics, shader->GetType() == eShaderType::Vertex);

			s_DeviceContext->IASetInputLayout(shader->GetInputLayout());
			s_DeviceContext->VSSetShader(shader->GetVertexShader(), nullptr, 0);
		}
		else
		{
			s_DeviceContext->IASetInputLayout(nullptr);
			s_DeviceContext->VSSetShader(nullptr, nullptr, 0);
		}

		s_CurrentVertexShader = shader;
	}

	void Graphics::SetHullShader(Shader* shader)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentHullShader == shader)
			return;

		if (shader)
		{
			DV_ASSERT(Graphics, shader->GetType() == eShaderType::Hull);
			s_DeviceContext->HSSetShader(shader->GetHullShader(), nullptr, 0);
		}
		else
			s_DeviceContext->HSSetShader(nullptr, nullptr, 0);

		s_CurrentHullShader = shader;
	}

	void Graphics::SetDomainShader(Shader* shader)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentDomainShader == shader)
			return;

		if (shader)
		{
			DV_ASSERT(Graphics, shader->GetType() == eShaderType::Domain);
			s_DeviceContext->DSSetShader(shader->GetDomainShader(), nullptr, 0);
		}
		else
			s_DeviceContext->DSSetShader(nullptr, nullptr, 0);

		s_CurrentDomainShader = shader;
	}

	void Graphics::SetGeometryShader(Shader* shader)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentGeometryShader == shader)
			return;

		if (shader)
		{
			DV_ASSERT(Graphics, shader->GetType() == eShaderType::Geometry);
			s_DeviceContext->GSSetShader(shader->GetGeometryShader(), nullptr, 0);
		}
		else
			s_DeviceContext->GSSetShader(nullptr, nullptr, 0);

		s_CurrentGeometryShader = shader;
	}

	void Graphics::SetPixelShader(Shader* shader)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentPixelShader == shader)
			return;

		if (shader)
		{
			DV_ASSERT(Graphics, shader->GetType() == eShaderType::Pixel);
			s_DeviceContext->PSSetShader(shader->GetPixelShader(), nullptr, 0);
		}
		else
			s_DeviceContext->PSSetShader(nullptr, nullptr, 0);

		s_CurrentPixelShader = shader;
	}

	void Graphics::SetComputeShader(Shader* shader)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentComputeShader == shader)
			return;

		if (shader)
		{
			DV_ASSERT(Graphics, shader->GetType() == eShaderType::Compute);
			s_DeviceContext->CSSetShader(shader->GetComputeShader(), nullptr, 0);
		}
		else
			s_DeviceContext->CSSetShader(nullptr, nullptr, 0);

		s_CurrentComputeShader = shader;
	}

	void Graphics::SetVertexBuffer(VertexBuffer* buffer)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentVertexBuffer == buffer)
			return;

		if (buffer)
		{
			ID3D11Buffer* vertexBuffer = buffer->GetBuffer();
			UINT stride = buffer->GetStride();
			UINT offset = 0;

			s_DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		}
		else
		{
			UINT stride = 0;
			UINT offset = 0;

			s_DeviceContext->IASetVertexBuffers(0, 1, nullptr, &stride, &offset);
		}

		s_CurrentVertexBuffer = buffer;
	} 
	
	void Graphics::SetIndexBuffer(IndexBuffer* buffer)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_CurrentIndexBuffer == buffer)
			return;

		if (buffer)
			s_DeviceContext->IASetIndexBuffer(buffer->GetBuffer(), buffer->GetFormat(), 0);
		else
			s_DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		s_CurrentIndexBuffer = buffer;
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY topology, UINT32 vertexCount, UINT32 vertexStart)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_PrimitiveTopology != topology)
		{
			s_DeviceContext->IASetPrimitiveTopology(topology);
			s_PrimitiveTopology = topology;
		}

		s_DeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, UINT32 indexCount, UINT32 indexStart)
	{
		DV_ASSERT(Graphics, s_DeviceContext);

		if (s_PrimitiveTopology != topology)
		{
			s_DeviceContext->IASetPrimitiveTopology(topology);
			s_PrimitiveTopology = topology;
		}

		s_DeviceContext->DrawIndexed(indexCount, indexStart, 0);
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

	ID3D11RenderTargetView* Graphics::GetRenderTargetView()
	{
		DV_ASSERT(Graphics, s_BackbufferRenderTarget);
		return s_BackbufferRenderTarget;
	}
	
	ID3D11DepthStencilView* Graphics::GetDepthStencilView()
	{
		DV_ASSERT(Graphics, s_BackbufferDepthStencil);
		return s_BackbufferDepthStencil;
	}

	void Graphics::updateBackbuffer()
	{
		DV_ASSERT(Graphics, s_SwapChain);
		DV_ASSERT(Graphics, s_Device);

		DV_RELEASE(s_BackbufferRenderTarget);
		DV_RELEASE(s_BackbufferTexture);
		DV_RELEASE(s_BackbufferDepthStencil);

		ID3D11Texture2D* pBackbufferTexture{};
		if (FAILED(s_SwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 텍스쳐 획득 실패");
			return;
		}

		if (FAILED(s_Device->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), 
			nullptr, 
			&s_BackbufferRenderTarget))) 
		{
			DV_RELEASE(s_BackbufferRenderTarget);
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
			static_cast<ID3D11Resource*>(s_BackbufferTexture),
			&viewDesc,
			&s_BackbufferDepthStencil))) 
		{
			DV_LOG(Graphics, err, "후면버퍼 깊이 스텐실 뷰 생성 실패");
			return;
		}

		s_ResolutionWidth = desc.BufferDesc.Width;
		s_ResolutionHeight = desc.BufferDesc.Height;
	}
}
