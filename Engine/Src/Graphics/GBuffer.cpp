#include "divepch.h"
#include "GBuffer.h"
#include "Graphics.h"
#include "RenderTexture.h"
#include "Core/CoreDefs.h"


namespace Dive
{
	GBuffer::GBuffer()
		: m_pGraphics(Graphics::GetInstance())
		, m_pAlbedoTex(nullptr)
		, m_pNormalTex(nullptr)
		, m_pSpecularTex(nullptr)
		, m_pDepthTex(nullptr)
	{
	}
	
	GBuffer::~GBuffer()
	{
		DV_DELETE(m_pDepthTex);
		DV_DELETE(m_pSpecularTex);
		DV_DELETE(m_pNormalTex);
		DV_DELETE(m_pAlbedoTex);
	}

	bool GBuffer::Initialize(uint32_t width, uint32_t height)
	{
		DV_ENGINE_ASSERT(width != 0 && height != 0);

		m_pAlbedoTex = new RenderTexture();
		if(!m_pAlbedoTex->CreateRenderTargetView(width, height))
			return false;

		m_pNormalTex = new RenderTexture();
		if (!m_pNormalTex->CreateRenderTargetView(width, height, DXGI_FORMAT_R11G11B10_FLOAT))
			return false;

		m_pSpecularTex = new RenderTexture();
		if (!m_pSpecularTex->CreateRenderTargetView(width, height))
			return false;

		m_pDepthTex = new RenderTexture();
		if (!m_pDepthTex->CreateDepthStencilView(width, height, DXGI_FORMAT_R24G8_TYPELESS))
			return false;

		return true;
	}
	
	void GBuffer::BindToRenderTargets()
	{
		m_pGraphics->BindRenderTargetView(m_pAlbedoTex->GetRenderTargetView(), 0);
		m_pGraphics->BindRenderTargetView(m_pNormalTex->GetRenderTargetView(), 1);
		m_pGraphics->BindRenderTargetView(m_pSpecularTex->GetRenderTargetView(), 2);
		m_pGraphics->BindDepthStencilView(m_pDepthTex->GetDepthStencilView());
	}
	
	void GBuffer::BindToResourceViews()
	{
		m_pGraphics->BindPSResource(m_pAlbedoTex->GetShaderResourceView(), eTextureUnitType::GBuffer_Color_SpecIntensity);
		m_pGraphics->BindPSResource(m_pNormalTex->GetShaderResourceView(), eTextureUnitType::GBuffer_Normal);
		m_pGraphics->BindPSResource(m_pSpecularTex->GetShaderResourceView(), eTextureUnitType::GBuffer_SpecPower);
		m_pGraphics->BindPSResource(m_pDepthTex->GetShaderResourceView(), eTextureUnitType::GBuffer_DepthStencil);
	}
}