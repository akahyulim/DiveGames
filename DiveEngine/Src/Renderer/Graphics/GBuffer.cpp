#include "GBuffer.h"
#include "../Renderer.h"
#include "../DiveCore.h"
#include "../Log.h"

// 1. Editor의 경우 View의 크기가 Resolution과 다르다. 즉, 따로 설정해야 한다.
//	- 굳이 GBuffer의 크기를 변경할 필요가 있을까? View 크기에 맞는 RenderTarget을 만든 후 결과를 그리면 되지 않나?
//		- 그렇다면 샘플링이 따로 필요한가?
// 2. Editor의 View, 그리고 후처리에 사용될 RenderTarget이 필요하다.
namespace dive
{
	GBuffer::GBuffer()
	{
	}

	GBuffer::~GBuffer()
	{
		Clear();
	}
	
	bool GBuffer::Initialize(ID3D11Device* pDevice, unsigned int width, unsigned int height)
	{
		DV_ASSERT(pDevice != nullptr);

		Clear();

		// 크기 확인 필요?
		m_Width = width;
		m_Height = height;

		// RT0 Formats: Base Color(RGB), Specular Intensity(A)
		static const DXGI_FORMAT ResourceFormatRT0 = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT RenderTargetViewFormatRT0 = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT ShaderResourceViewFormatRT0 = DXGI_FORMAT_R8G8B8A8_UNORM;

		// RT1 Formats: Normal(RGB)
		static const DXGI_FORMAT ResourceFormatRT1 = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT RenderTargetViewFormatRT1 = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT ShaderResourceViewFormatRT1 = DXGI_FORMAT_R11G11B10_FLOAT;

		// RT2 Formats: Specular Power(R)
		static const DXGI_FORMAT ResourceFormatRT2 = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT RenderTargetViewFormatRT2 = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT ShaderResourceViewFormatRT2 = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Depth Stencil Formats
		static const DXGI_FORMAT ResourceFormatDepthStencil = DXGI_FORMAT_R24G8_TYPELESS;
		static const DXGI_FORMAT depthStencilViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		static const DXGI_FORMAT depthStencilShaderResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;


		// create textures
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			desc.Format = ResourceFormatDepthStencil;
			if (FAILED(pDevice->CreateTexture2D(&desc, nullptr, m_pResourceDepthStencil.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
			
			desc.Format  = ResourceFormatRT0;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			if (FAILED(pDevice->CreateTexture2D(&desc, nullptr, m_pResourceRT0.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = ResourceFormatRT1;
			if (FAILED(pDevice->CreateTexture2D(&desc, nullptr, m_pResourceRT1.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = ResourceFormatRT2;
			if (FAILED(pDevice->CreateTexture2D(&desc, nullptr, m_pResourceRT2.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// create Render Target View
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			desc.Format = RenderTargetViewFormatRT0;
			if (FAILED(pDevice->CreateRenderTargetView(m_pResourceRT0.Get(), &desc, m_pRenderTargetViewRT0.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = RenderTargetViewFormatRT1;
			if (FAILED(pDevice->CreateRenderTargetView(m_pResourceRT1.Get(), &desc, m_pRenderTargetViewRT1.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = RenderTargetViewFormatRT2;
			if (FAILED(pDevice->CreateRenderTargetView(m_pResourceRT2.Get(), &desc, m_pRenderTargetViewRT2.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// create Shader Resource View
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;

			desc.Format = depthStencilShaderResourceViewFormat;
			if (FAILED(pDevice->CreateShaderResourceView(m_pResourceDepthStencil.Get(), &desc, m_pShaderResourceViewDepthStencil.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = ShaderResourceViewFormatRT0;
			if (FAILED(pDevice->CreateShaderResourceView(m_pResourceRT0.Get(), &desc, m_pShaderResourceViewRT0.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = ShaderResourceViewFormatRT1;
			if (FAILED(pDevice->CreateShaderResourceView(m_pResourceRT1.Get(), &desc, m_pShaderResourceViewRT1.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = ShaderResourceViewFormatRT2;
			if (FAILED(pDevice->CreateShaderResourceView(m_pResourceRT2.Get(), &desc, m_pShaderResourceViewRT2.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}
		
		// create Depth Stencil View
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = depthStencilViewFormat;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Flags = 0;

			if (FAILED(pDevice->CreateDepthStencilView(m_pResourceDepthStencil.Get(), &desc, m_pDepthStencilView.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
			if (FAILED(pDevice->CreateDepthStencilView(m_pResourceDepthStencil.Get(), &desc, m_pDepthStencilViewReadOnly.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// depth stencil state
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
			desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
			const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
			desc.FrontFace = stencilMarkOp;
			desc.BackFace = stencilMarkOp;

			if (FAILED(pDevice->CreateDepthStencilState(&desc, m_pDepthStencilState.GetAddressOf())))
			{
				CORE_ERROR("");
				return false;
			}
		}

		CORE_TRACE("GBuffer Initialized - Resolution {:d} x {:d}", m_Width, m_Height);

		return true;
	}
	
	void GBuffer::Clear()
	{
		// depth stencil state
		m_pDepthStencilState.Reset();

		// shader resource views
		m_pShaderResourceViewDepthStencil.Reset();
		m_pShaderResourceViewRT2.Reset();
		m_pShaderResourceViewRT1.Reset();
		m_pShaderResourceViewRT0.Reset();

		// render target views
		m_pRenderTargetViewRT2.Reset();
		m_pRenderTargetViewRT1.Reset();
		m_pRenderTargetViewRT0.Reset();

		// depth stencil views
		m_pDepthStencilViewReadOnly.Reset();
		m_pDepthStencilView.Reset();

		// resources
		m_pResourceRT2.Reset();
		m_pResourceRT1.Reset();
		m_pResourceRT0.Reset();
		m_pResourceDepthStencil.Reset();
	}
	
	void GBuffer::PreRender(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		// DepthStencilView
		pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// RenderTargetView
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		pImmediateContext->ClearRenderTargetView(m_pRenderTargetViewRT0.Get(), clearColor);
		pImmediateContext->ClearRenderTargetView(m_pRenderTargetViewRT1.Get(), clearColor);
		pImmediateContext->ClearRenderTargetView(m_pRenderTargetViewRT2.Get(), clearColor);

		ID3D11RenderTargetView* pRT[3] = { m_pRenderTargetViewRT0.Get(), m_pRenderTargetViewRT1.Get(), m_pRenderTargetViewRT2.Get() };
		pImmediateContext->OMSetRenderTargets(3, pRT, m_pDepthStencilView.Get());

		pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 1);
	}
	
	void GBuffer::PostRender(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		ID3D11RenderTargetView* pRT[3] = { nullptr, nullptr, nullptr };
		pImmediateContext->OMSetRenderTargets(3, pRT, m_pDepthStencilViewReadOnly.Get());
	}
	
	void GBuffer::PrepareForUnpack(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		// ConstantBuffer를 map / unmap하여 투영 행렬과 뷰 역행렬을 계산한 후 저장한다.
		// 그리고 PS에 Set한다.
	}
}