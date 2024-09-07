#include "divepch.h"
#include "GBuffer.h"
#include "Graphics.h"
#include "RenderTexture.h"
#include "Core/CoreDefs.h"


namespace Dive
{
	GBuffer::GBuffer()
		: m_pGraphics(Graphics::GetInstance())
		, m_pDiffuseTex(nullptr)
		, m_pNormalTex(nullptr)
		, m_pSpecularTex(nullptr)
		, m_pDepthTex(nullptr)
		, m_Width(0)
		, m_Height(0)
	{
	}
	
	GBuffer::~GBuffer()
	{
		Release();
	}

	bool GBuffer::Initialize(uint32_t width, uint32_t height)
	{
		DV_ENGINE_ASSERT(width != 0 && height != 0);

		Release();

		m_pDiffuseTex = new RenderTexture(width, height);
		if (!m_pDiffuseTex->Create())
			return false;

		m_pNormalTex = new RenderTexture(width, height, DXGI_FORMAT_R11G11B10_FLOAT);
		if (!m_pNormalTex->Create())
			return false;

		m_pSpecularTex = new RenderTexture(width, height);
		if (!m_pSpecularTex->Create())
			return false;

		m_pDepthTex = new RenderTexture(width, height, 24, true);
		if (!m_pDepthTex->Create())
			return false;

		m_Width = width;
		m_Height = height;
		 
		return true;
	}

	void GBuffer::Release()
	{
		DV_DELETE(m_pDepthTex);
		DV_DELETE(m_pSpecularTex);
		DV_DELETE(m_pNormalTex);
		DV_DELETE(m_pDiffuseTex);
	}
}