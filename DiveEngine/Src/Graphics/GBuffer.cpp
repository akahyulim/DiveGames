#include "DivePch.h"
#include "GBuffer.h"
#include "Graphics.h"
#include "RenderTexture.h"
#include "Core/CoreDefs.h"
#include "IO/Log.h"

namespace Dive
{
	GBuffer::GBuffer()
		: m_pDepthStencil(nullptr),
		m_pColorSpecIntensity(nullptr),
		m_pNormal(nullptr),
		m_pSpecPower(nullptr),
		m_pGBufferUnpackCB(nullptr),
		m_pDepthStencilState(nullptr)
	{
	}

	GBuffer::~GBuffer()
	{
		Destroy();
	}

	bool GBuffer::Create(unsigned int width, unsigned int height)
	{
		DV_ASSERT(Graphics::IsInitialized());

		Destroy();

		// depth stencil
		m_pDepthStencil = new RenderTexture;
		if (!m_pDepthStencil->SetDepthStencil(width, height, DXGI_FORMAT_R24G8_TYPELESS, true))
		{
			DV_CORE_ERROR("GBuffer - DepthStencil Texture 생성에 실패하였습니다.");
			return false;
		}
		m_pDepthStencil->SetFilter(D3D11_FILTER_MIN_MAG_MIP_POINT);
		m_pDepthStencil->UpdateSamplerState();

		// color & spec intensity
		m_pColorSpecIntensity = new RenderTexture;
		if (!m_pColorSpecIntensity->SetRenderTarget(width, height))
		{
			DV_CORE_ERROR("GBuffer - ColorSpecIntensity Texture 생성에 실패하였습니다.");
			return false;
		}
		m_pColorSpecIntensity->UpdateSamplerState();

		// normal
		m_pNormal = new RenderTexture;
		if (!m_pNormal->SetRenderTarget(width, height, DXGI_FORMAT_R11G11B10_FLOAT))
		{
			DV_CORE_ERROR("GBuffer - Normal Texture 생성에 실패하였습니다.");
			return false;
		}
		m_pNormal->UpdateSamplerState();

		// spec power
		m_pSpecPower = new RenderTexture;
		if (!m_pSpecPower->SetRenderTarget(width, height))
		{
			DV_CORE_ERROR("GBuffer - SpecPower Texture 생성에 실패하였습니다.");
			return false;
		}
		m_pSpecPower->UpdateSamplerState();

		auto pDevice = Graphics::GetDevice();

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

			if (FAILED(pDevice->CreateDepthStencilState(&desc, &m_pDepthStencilState)))
			{
				DV_CORE_ERROR("GBuffer 구성 중 DepthStencilState 생성에 실패하였습니다.");
				return false;
			}
		}

		return true;
	}
	
	void GBuffer::Destroy()
	{
		DV_DELETE(m_pSpecPower);
		DV_DELETE(m_pNormal);
		DV_DELETE(m_pColorSpecIntensity);
		DV_DELETE(m_pDepthStencil);

		DV_RELEASE(m_pGBufferUnpackCB);
		DV_RELEASE(m_pDepthStencilState);
	}
	
	void GBuffer::PreRender()
	{
		DV_ASSERT(Graphics::IsInitialized());

		ID3D11RenderTargetView* pRenderTextures[] = { 
			m_pColorSpecIntensity->GetRenderTargetView(), 
			m_pNormal->GetRenderTargetView(), 
			m_pSpecPower->GetRenderTargetView() };
		Graphics::SetRenderTargetViews(0, 3, pRenderTextures);
		Graphics::SetDepthStencilView(m_pDepthStencil->GetDepthStencilView());
		Graphics::ClearViews(eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 1.0, 0);
		Graphics::SetDepthStencilState(m_pDepthStencilState, 1);
	}
	
	void GBuffer::PostRender()
	{
		ID3D11RenderTargetView* pRenderTargetViews[] = { nullptr, nullptr, nullptr };
		Graphics::SetRenderTargetViews(0, 3, pRenderTargetViews);
		Graphics::SetDepthStencilView(m_pDepthStencil->GetDepthStencilViewReadOnly());
	}

	void GBuffer::PrepareForUnpack()
	{
		// perpective, view matrix가 필요하므로 camera에 접근가능해야 한다.
		// 혹은 다른 곳에서 이미 전달한 값을 활용할 수도 있다.
	}

	ID3D11DepthStencilView* GBuffer::GetDepthStencilView() const
	{
		return m_pDepthStencil ? m_pDepthStencil->GetDepthStencilView() : nullptr;
	}

	ID3D11DepthStencilView* GBuffer::GetDepthSteniclViewReadOnly() const
	{
		return m_pDepthStencil ? m_pDepthStencil->GetDepthStencilViewReadOnly() : nullptr;
	}

	ID3D11ShaderResourceView* GBuffer::GetDepthStencilSRV() const
	{
		return m_pDepthStencil ? m_pDepthStencil->GetShaderResourceView(): nullptr;
	}

	ID3D11ShaderResourceView* GBuffer::GetColorSpecIntensitySRV() const
	{
		return m_pColorSpecIntensity ? m_pColorSpecIntensity->GetShaderResourceView() : nullptr;
	}
	
	ID3D11ShaderResourceView* GBuffer::GetNormalSRV() const
	{
		return m_pNormal ? m_pNormal->GetShaderResourceView() : nullptr;
	}
	
	ID3D11ShaderResourceView* GBuffer::GetSpecPowerSRV() const
	{
		return m_pSpecPower ? m_pSpecPower->GetShaderResourceView() : nullptr;
	}

	RenderTexture* GBuffer::GetDepthStencilTexture() const
	{
		return m_pDepthStencil;
	}
	
	RenderTexture* GBuffer::GetColorSpecIntTexture() const
	{
		return m_pColorSpecIntensity;
	}
	
	RenderTexture* GBuffer::GetNormalTexture() const
	{
		return m_pNormal;
	}
	
	RenderTexture* GBuffer::GetSpecPowTexture() const
	{
		return m_pSpecPower;
	}
}