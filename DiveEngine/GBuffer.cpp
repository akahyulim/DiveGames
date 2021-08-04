#include "GBuffer.h"
#include "DiveCore.h"
#include "Log.h"

// 유니티의 RenderTarget은 GBuffer의 Texture로 사용하기 힘들다.
// 그리고 스파르탄처럼 Texture를 만드는 것도 에바 같다.
// 그래서 일단 책을 따르기로 했다.
namespace dive
{
	GBuffer::GBuffer(ID3D11Device* pDevice)
	{
		DV_ASSERT(pDevice != nullptr);
		m_pDevice = pDevice;
	}

	GBuffer::~GBuffer()
	{
		Clear();
	}
	
	// Resize시에도 호출된다.
	bool GBuffer::Initialize(unsigned int width, unsigned int height)
	{
		Clear();

		// 크기 확인 필요?

		// Texture formats
		static const DXGI_FORMAT depthStencilTextureFormat = DXGI_FORMAT_R24G8_TYPELESS;
		static const DXGI_FORMAT colorSpecpIntesityTextureForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT normalTextureFormat = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT specPowerTextureFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Rener Target View formats
		static const DXGI_FORMAT depthStencilViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		static const DXGI_FORMAT colorSpecpIntesityRenderTargetViewForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT normalRenderTargetViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT specPowerRenderTargetViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		
		// Shader Resource View formats
		static const DXGI_FORMAT depthStencilShaderResourceViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		static const DXGI_FORMAT colorSpecpIntesityShaderResourceViewForamt = DXGI_FORMAT_R8G8B8A8_UNORM;
		static const DXGI_FORMAT normalShaderResourceViewFormat = DXGI_FORMAT_R11G11B10_FLOAT;
		static const DXGI_FORMAT specPowerShaderResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

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

			desc.Format = depthStencilTextureFormat;
			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexDepthStencil)))
			{
				CORE_ERROR("");
				return false;
			}
			
			desc.Format  = colorSpecpIntesityTextureForamt;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexColorSpecIntensity)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = normalTextureFormat;
			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexNormal)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = specPowerTextureFormat;
			if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, &m_pTexSpecPower)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// create DSV
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = depthStencilViewFormat;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc.Flags = 0;

			if (FAILED(m_pDevice->CreateDepthStencilView(m_pTexDepthStencil, &desc, &m_pDepthStencilView)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Flags = D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL;
			if (FAILED(m_pDevice->CreateDepthStencilView(m_pTexDepthStencil, &desc, &m_pReadOnlyDepthStencilView)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// create RTV
		{
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			desc.Format = colorSpecpIntesityRenderTargetViewForamt;
			if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexColorSpecIntensity, &desc, &m_pColorSpecIntensityRTV)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = normalRenderTargetViewFormat;
			if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexNormal, &desc, &m_pNormalRTV)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = specPowerRenderTargetViewFormat;
			if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexSpecPower, &desc, &m_pSpecPowerRTV)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// create SRV
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;

			desc.Format = depthStencilShaderResourceViewFormat;
			if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexDepthStencil, &desc, &m_pDepthStencilSRV)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = colorSpecpIntesityShaderResourceViewForamt;
			if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexColorSpecIntensity, &desc, &m_pColorSpecIntensitySRV)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = normalShaderResourceViewFormat;
			if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexNormal, &desc, &m_pNormalSRV)))
			{
				CORE_ERROR("");
				return false;
			}

			desc.Format = specPowerShaderResourceViewFormat;
			if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexSpecPower, &desc, &m_pSpecPowerSRV)))
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

			if (FAILED(m_pDevice->CreateDepthStencilState(&desc, &m_pDepthStencilState)))
			{
				CORE_ERROR("");
				return false;
			}
		}

		// constant buffer
		{

		}

		return true;
	}
	
	void GBuffer::Clear()
	{
		DV_RELEASE(m_pTexDepthStencil);
		DV_RELEASE(m_pTexColorSpecIntensity);
		DV_RELEASE(m_pTexNormal);
		DV_RELEASE(m_pTexSpecPower);

		DV_RELEASE(m_pDepthStencilView);
		DV_RELEASE(m_pReadOnlyDepthStencilView);
		DV_RELEASE(m_pColorSpecIntensityRTV);
		DV_RELEASE(m_pNormalRTV);
		DV_RELEASE(m_pSpecPowerRTV);

		DV_RELEASE(m_pDepthStencilSRV);
		DV_RELEASE(m_pColorSpecIntensitySRV);
		DV_RELEASE(m_pNormalSRV);
		DV_RELEASE(m_pSpecPowerRTV);

		DV_RELEASE(m_pDepthStencilState);
	}
	
	void GBuffer::PreRender(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		// DepthStencilView
		pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// RenderTargetView
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		pImmediateContext->ClearRenderTargetView(m_pColorSpecIntensityRTV, clearColor);
		pImmediateContext->ClearRenderTargetView(m_pNormalRTV, clearColor);
		pImmediateContext->ClearRenderTargetView(m_pSpecPowerRTV, clearColor);

		ID3D11RenderTargetView* pRT[3] = { m_pColorSpecIntensityRTV, m_pNormalRTV, m_pSpecPowerRTV };
		pImmediateContext->OMSetRenderTargets(3, pRT, m_pDepthStencilView);

		pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	}
	
	void GBuffer::PostRender(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		ID3D11RenderTargetView* pRT[3] = { nullptr, nullptr, nullptr };
		pImmediateContext->OMSetRenderTargets(3, pRT, m_pReadOnlyDepthStencilView);
	}
	
	void GBuffer::PrepareForUnpack(ID3D11DeviceContext* pImmediateContext)
	{
		DV_ASSERT(pImmediateContext);

		// ConstantBuffer를 map / unmap하여 투영 행렬과 뷰 역행렬을 계산한 후 저장한다.
		// 그리고 PS에 Set한다.
	}
}