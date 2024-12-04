#include "stdafx.h"
#include "GBuffer.h"
#include "Texture2D.h"
#include "Core/CoreDefs.h"

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
		Release();
	}

	bool GBuffer::Initialize(uint32_t width, uint32_t height)
	{
		DV_ASSERT(GBuffer, width != 0 && height != 0);

		Release();

		m_pDiffuseTex = new Texture2D(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
		m_pNormalTex = new Texture2D(width, height, DXGI_FORMAT_R11G11B10_FLOAT, false);
		m_pSpecularTex = new Texture2D(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, false);
		m_pDepthTex = new Texture2D(width, height, 24, true);

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