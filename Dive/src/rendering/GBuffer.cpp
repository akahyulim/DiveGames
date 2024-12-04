#include "stdafx.h"
#include "GBuffer.h"
#include "Core/CoreDefs.h"
#include "graphics/Texture2D.h"

namespace Dive
{
	GBuffer::GBuffer()
		: m_Width(0)
		, m_Height(0)
		, m_pDiffuseTex(nullptr)
		, m_pNormalTex(nullptr)
		, m_pSpecularTex(nullptr)
		, m_pDepthTex(nullptr)
	{
	}
	
	GBuffer::~GBuffer()
	{
		Shutdown();
	}

	bool GBuffer::Resize(uint32_t width, uint32_t height)
	{
		DV_ASSERT(GBuffer, width != 0 && height != 0);

		Shutdown();

		m_pDiffuseTex = new Texture2D(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
		m_pNormalTex = new Texture2D(width, height, DXGI_FORMAT_R11G11B10_FLOAT, false);
		m_pSpecularTex = new Texture2D(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
		m_pDepthTex = new Texture2D(width, height, 24, true);

		m_Width = width;
		m_Height = height;
		 
		return true;
	}

	void GBuffer::Shutdown()
	{
		DV_DELETE(m_pDepthTex);
		DV_DELETE(m_pSpecularTex);
		DV_DELETE(m_pNormalTex);
		DV_DELETE(m_pDiffuseTex);
	}
}