#include "divepch.h"
#include "Graphics.h"
#include "Shader.h"
#include "Texture2D.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Core/CoreDefs.h"

namespace Dive
{
	IDXGISwapChain* Graphics::s_pSwapChain = nullptr;
	ID3D11Device* Graphics::s_pDevice = nullptr;
	ID3D11DeviceContext* Graphics::s_pDeviceContext = nullptr;

	ID3D11RenderTargetView* Graphics::s_pDefaultRenderTargetView = nullptr;
	ID3D11DepthStencilView* Graphics::s_pDefaultDepthStencilView = nullptr;
	ID3D11Texture2D* Graphics::s_pDefaultDepthTexture = nullptr;

	uint32_t Graphics::s_BackbufferWidth = 0;
	uint32_t Graphics::s_BackbufferHeight = 0;

	bool Graphics::s_bVSync = false;

	D3D11_PRIMITIVE_TOPOLOGY Graphics::s_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	ID3D11RenderTargetView* Graphics::s_RenderTargetViews[4] = { nullptr, };
	ID3D11DepthStencilView* Graphics::s_pDepthStencilView = nullptr;

	bool Graphics::s_bRenderTargetViewsDirty = true;

	Shader* Graphics::s_pShader = nullptr;

	VertexBuffer* Graphics::s_pVertexBuffer = nullptr;
	IndexBuffer* Graphics::s_pIndexBuffer = nullptr;

	ID3D11DepthStencilState* Graphics::s_pDepthStencilState = nullptr;
	ID3D11RasterizerState* Graphics::s_pRasterizerState = nullptr;
	ID3D11BlendState* Graphics::s_pBlendState = nullptr;

	ID3D11ShaderResourceView* Graphics::s_ShaderResourceViews[(uint32_t)eTextureUnit::Max_Num] = { nullptr, };
	ID3D11SamplerState* Graphics::s_SamplerStates[(uint32_t)eTextureUnit::Max_Num] = { nullptr, };

	bool Graphics::s_bTextureDirty = true;

	bool Graphics::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool fullScreen)
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		if (FAILED(D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			&featureLevel,//nullptr,
			1, //0,
			D3D11_SDK_VERSION,
			&s_pDevice,
			nullptr,
			&s_pDeviceContext)))
		{
			DV_CORE_ERROR("ID3D11Device & ID3D11DeviceContext ������ �����Ͽ����ϴ�.");
			return false;
		}

		IDXGIDevice* pDxgiDevice = nullptr;
		s_pDevice->QueryInterface(IID_IDXGIDevice, (void**)&pDxgiDevice);
		IDXGIAdapter* pDxgiAdapter = nullptr;
		pDxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&pDxgiAdapter);
		IDXGIFactory* pDxgiFactory = nullptr;
		pDxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&pDxgiFactory);

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BufferCount = 1;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// sRGB???
		desc.BufferDesc.RefreshRate.Denominator = 1;			// ���� ����(vsync�� ���� �޶�����?)
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;								// ��Ƽ ���ø� off
		desc.SampleDesc.Quality = 0;
		desc.Windowed = true;
		desc.OutputWindow = hWnd;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// rastertek���� 0�̰� �ٸ� ���� ������ ���� �ִ�...

		if (FAILED(pDxgiFactory->CreateSwapChain(s_pDevice, &desc, &s_pSwapChain)))
		{
			DV_CORE_ERROR("IDXGISwapchain ������ �����Ͽ����ϴ�.");
			return false;
		}

		if (!UpdateSwapChain(width, height))
		{
			DV_CORE_ERROR("�ĸ���� ���ſ� �����Ͽ����ϴ�.");
			return false;
		}

		return true;
	}

	void Graphics::Shutdown()
	{
		DV_RELEASE(s_pDefaultDepthTexture);
		DV_RELEASE(s_pDefaultDepthStencilView);
		DV_RELEASE(s_pDefaultRenderTargetView);

		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pDeviceContext);
		DV_RELEASE(s_pSwapChain);
	}

	bool Graphics::UpdateSwapChain(uint32_t width, uint32_t height)
	{
		if (s_BackbufferWidth == width && s_BackbufferHeight == height)
			return true;

		s_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		DV_RELEASE(s_pDefaultRenderTargetView);
		DV_RELEASE(s_pDefaultDepthTexture);
		DV_RELEASE(s_pDefaultDepthStencilView);

		s_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		ID3D11Texture2D* pBackbufferTexture = nullptr;
		if (FAILED(s_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackbufferTexture)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_CORE_ERROR("�ĸ� ���� �ؽ��ĸ� �����µ� �����Ͽ����ϴ�.");
			return false;
		}

		if (FAILED(s_pDevice->CreateRenderTargetView(
			static_cast<ID3D11Resource*>(pBackbufferTexture), nullptr, &s_pDefaultRenderTargetView)))
		{
			DV_RELEASE(pBackbufferTexture);
			DV_RELEASE(s_pDefaultRenderTargetView);
			DV_CORE_ERROR("�ĸ� ���� ����Ÿ�ٺ� ������ �����Ͽ����ϴ�.");
			return false;
		}
		DV_RELEASE(pBackbufferTexture);

		uint32_t curWidth = width;
		uint32_t curHeight = height;

		if (width == 0 || height == 0)
		{
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
			s_pSwapChain->GetDesc(&swapChainDesc);

			curWidth = swapChainDesc.BufferDesc.Width;
			curHeight = swapChainDesc.BufferDesc.Height;
		}

		D3D11_TEXTURE2D_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(texDesc));
		texDesc.Width = curWidth;
		texDesc.Height = curHeight;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texDesc.SampleDesc.Count = 1;// static_cast<UINT>(screenParams_.multiSample_);
		texDesc.SampleDesc.Quality = 0;//impl->GetMultiSampleQuality(texDesc.Format, screenParams_.multiSample_);
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		if (FAILED(s_pDevice->CreateTexture2D(&texDesc, nullptr, &s_pDefaultDepthTexture)))
		{
			DV_RELEASE(s_pDefaultDepthTexture);
			DV_CORE_ERROR("�ĸ� ���� ���� ���ٽ� �ؽ��� ������ �����Ͽ����ϴ�.");
			return false;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		ZeroMemory(&viewDesc, sizeof(viewDesc));
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;

		if (FAILED(s_pDevice->CreateDepthStencilView(
			static_cast<ID3D11Resource*>(s_pDefaultDepthTexture),
			&viewDesc,//nullptr,		// urho�� nullptr�� �����ߴ�. ���ó� sampler ������ �ذ���� �ʾҴ�.
			&s_pDefaultDepthStencilView)))
		{
			DV_RELEASE(s_pDefaultDepthStencilView);
			DV_CORE_ERROR("�ĸ� ���� ���� ���ٽ� �� ������ �����Ͽ����ϴ�.");
			return false;
		}

		s_BackbufferWidth = curWidth;
		s_BackbufferHeight = curHeight;

		resetViews();

		return true;
	}

	bool Graphics::IsInitialized()
	{
		return s_pDevice != nullptr;
	}

	bool Graphics::BeginFrame()
	{
		if(!IsInitialized())
			return false;

		resetViews();

		// Max_Num�� constexptr�� �ϴ°� ���ƺ��δ�?
		for (uint32_t i = 0; i < (uint32_t)eTextureUnit::Max_Num; i++)
			SetTexture((eTextureUnit)i, nullptr);

		return true;
	}

	void Graphics::EndFrame()
	{
		if (!IsInitialized())
			return;

		s_pSwapChain->Present(s_bVSync ? 1 : 0, 0);
	}

	void Graphics::ClearViews(uint8_t flags, const float* pColor, float depth, uint8_t stencil)
	{
		// �� ������ SetRenderTarget�� SetDepthStencil�� ��Ƽüũ �� ���ε��� �ϱ� �����̴�.
		// ���� �̸��� �� ��︮�� �ʰ� �Ǿ���.
		// ��¼�� �ش� �κ��� �и��ϴ� ���� �� �������� ���� �� ����.
		prepareDraw();

		if (flags & eClearFlags::Color)
			s_pDeviceContext->ClearRenderTargetView(s_RenderTargetViews[0], pColor);
		
		if ((flags & eClearFlags::Depth) || (flags & eClearFlags::Stencil))
		{
			uint32_t clearFlags = 0;
			clearFlags |= (flags & eClearFlags::Depth) ? D3D11_CLEAR_DEPTH : 0;
			clearFlags |= (flags & eClearFlags::Stencil) ? D3D11_CLEAR_STENCIL : 0;

			s_pDeviceContext->ClearDepthStencilView(s_pDepthStencilView, clearFlags, depth, stencil);
		}
	}

	void Graphics::SetRenderTargetView(uint32_t index, ID3D11RenderTargetView* pRenderTargetView)
	{
		// ���� ����� constexpr�� �ٲٱ�
		if (index >= 4)
			return;

		if (pRenderTargetView != s_RenderTargetViews[index])
		{
			s_RenderTargetViews[index] = pRenderTargetView;
			s_bRenderTargetViewsDirty = true;
		}
	}

	// read only ������ ���� �ָ�������. 
	// RenderTexture�� �޾ƾ� �ϳ�...?
	void Graphics::SetDepthStencilView(ID3D11DepthStencilView* pDepthStencilView)
	{
		if (pDepthStencilView != s_pDepthStencilView)
		{
			s_pDepthStencilView = pDepthStencilView;
			s_bRenderTargetViewsDirty = true;
		}
	}

	void Graphics::SetViewport(const RECT& rt)
	{
		static D3D11_VIEWPORT viewport;
		viewport.TopLeftX = (float)rt.left;
		viewport.TopLeftY = (float)rt.top;
		viewport.Width = (float)(rt.right - rt.left);
		viewport.Height = (float)(rt.bottom - rt.top);
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;

		s_pDeviceContext->RSSetViewports(1, &viewport);
	}

	void Graphics::SetDepthStencilState(ID3D11DepthStencilState* pState)
	{
		if (pState != s_pDepthStencilState)
		{
			s_pDeviceContext->OMSetDepthStencilState(pState, 1);
			s_pDepthStencilState = pState;
		}
	}

	void Graphics::SetRasterizerState(ID3D11RasterizerState* pState)
	{
		if (pState != s_pRasterizerState)
		{
			s_pDeviceContext->RSSetState(pState);
			s_pRasterizerState = pState;
		}
	}

	void Graphics::SetBlendState(ID3D11BlendState* pState)
	{
		if (pState != s_pBlendState)
		{
			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			s_pDeviceContext->OMSetBlendState(pState, blendFactor, 0xffffffff);
			s_pBlendState = pState;
		}
	}

	void Graphics::SetTexture(eTextureUnit index, Texture* pTexture)
	{
		if (pTexture)
		{
			if (pTexture->GetShaderResourceView() != s_ShaderResourceViews[(uint32_t)index])
			{
				if (pTexture->IsMipLevelsDirty())
					pTexture->UpdateMipLevels();

				if (pTexture->IsSamplerStateDirty())
					pTexture->UpdateSamplerState();

				s_ShaderResourceViews[(uint32_t)index] = pTexture->GetShaderResourceView();
				s_SamplerStates[(uint32_t)index] = pTexture->GetSamplerState();

				s_bTextureDirty = true;
			}
		}
		else
		{
			if (s_ShaderResourceViews[(uint32_t)index])
			{
				s_ShaderResourceViews[(uint32_t)index] = nullptr;
				s_SamplerStates[(uint32_t)index] = nullptr;

				s_bTextureDirty = true;
			}
		}
	}

	void Graphics::SetShader(Shader* pShader)
	{
		if (pShader != s_pShader)
		{
			s_pDeviceContext->IASetInputLayout(pShader->GetInputLayout());
			s_pDeviceContext->VSSetShader(pShader->GetVertexShader(), nullptr, 0);
			s_pDeviceContext->PSSetShader(pShader->GetPixelShader(), nullptr, 0);

			s_pShader = pShader;
		}
	}

	// ���� ���۸� �ٷ�� �ִ�.
	void Graphics::SetVertexBuffer(VertexBuffer* pBuffer)
	{
		if (pBuffer != s_pVertexBuffer)
		{
			if (pBuffer)
				s_pDeviceContext->IASetVertexBuffers(0, 1, pBuffer->GetBuffer(), pBuffer->GetStride(), pBuffer->GetOffset());
			else
				s_pDeviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);

			s_pVertexBuffer = pBuffer;
		}
	}

	void Graphics::SetIndexBuffer(IndexBuffer* pBuffer)
	{
		if (pBuffer != s_pIndexBuffer)
		{
			if (pBuffer)
				s_pDeviceContext->IASetIndexBuffer(pBuffer->GetBuffer(), pBuffer->GetFormat(), 0);
			else
				s_pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

			s_pIndexBuffer = pBuffer;
		}
	}

	void Graphics::Draw(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t vertexCount, uint32_t vertexStart)
	{
		prepareDraw();

		if (s_PrimitiveTopology != topology)
		{
			s_pDeviceContext->IASetPrimitiveTopology(topology);
			s_PrimitiveTopology = topology;
		}

		s_pDeviceContext->Draw(vertexCount, vertexStart);
	}

	void Graphics::DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY topology, uint32_t indexCount, uint32_t indexStart)
	{
		prepareDraw();

		if (s_PrimitiveTopology != topology)
		{
			s_pDeviceContext->IASetPrimitiveTopology(topology);
			s_PrimitiveTopology = topology;
		}

		s_pDeviceContext->DrawIndexed(indexCount, indexStart, 0);
	}

	// ������ �̸��� ������ ���� �ʴ´�.
	// �ᱹ ��� bind�� �������� �ʴ´�.
	// ��Ƽüũ�� ������� ���濩�θ� Ȯ���ϰ� �ʿ��� ��� ���� �����ϴ� ���ҽ��� ó���ϴ� �Լ�����.
	// => SetRenderTargets, SetTexture�� �� ó���ϸ� �ʿ䰡 ������ �� ����.
	void Graphics::prepareDraw()
	{
		if (s_bRenderTargetViewsDirty)
		{
			// 1. DepthStencilView�� �����Ѵٸ� ���� ���ٸ� ����Ʈ ���̹���
			// 2. !���̾���, DepthSTencilView�� ReadOnly��� ReadOnly��...
			// 3. RenderTargets[0]�� �����, DepthStencil�� ���ų�, ������ ũ�Ⱑ ����۶� ���� �� ����Ʈ ����Ÿ��

			s_pDeviceContext->OMSetRenderTargets(4, &s_RenderTargetViews[0], s_pDefaultDepthStencilView);

			s_bRenderTargetViewsDirty = false;
		}

		if (s_bTextureDirty)
		{
			s_pDeviceContext->PSSetShaderResources(0, (uint32_t)eTextureUnit::Max_Num, &s_ShaderResourceViews[0]);
			s_pDeviceContext->PSSetSamplers(0, (uint32_t)eTextureUnit::Max_Num, &s_SamplerStates[0]);

			s_bTextureDirty = false;
		}
	}

	// beginFrame(), reiszeBackbuffer() ��� ȣ��
	void Graphics::resetViews()
	{
		for (uint32_t i = 0; i < 4; i++)
			SetRenderTargetView(i, nullptr);

		SetDepthStencilView(nullptr);

		RECT rt = { 0, 0, (LONG)s_BackbufferWidth, (LONG)s_BackbufferHeight };
		SetViewport(rt);
	}
}