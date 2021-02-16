#include "DivePch.h"
#include "GBuffer.h"
#include "Dive_Texture.h"
#include "ConstantBuffer.h"
#include "Core/DiveDefs.h"
#include "Core/Log.h"


namespace Dive
{
	GBuffer::GBuffer(Context* context)
		: Object(context),
		m_constantBuffer(nullptr),
		m_colorSpecIntensity(nullptr),
		m_normal(nullptr),
		m_specPower(nullptr),
		m_dpethStencilReadOnly(nullptr),
		m_width(0),
		m_height(0)
	{
	}

	GBuffer::~GBuffer()
	{
		Clear();
	}
	
	bool GBuffer::Initialize(unsigned int width, unsigned int height)
	{
		if ((width == 0) || (height == 0))
		{
			CORE_ERROR("");
			return false;
		}

		if ((m_height == height) && (m_width == width))
		{
			CORE_TRACE("");
			return true;
		}

		Clear();

		m_colorSpecIntensity = new Dive_Texture(m_context);
		m_colorSpecIntensity->CreateRenderTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

		m_normal = new Dive_Texture(m_context);
		m_normal->CreateRenderTexture(width, height, DXGI_FORMAT_R11G11B10_FLOAT);

		m_specPower = new Dive_Texture(m_context);
		m_specPower->CreateRenderTexture(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

		m_dpethStencilReadOnly = new Dive_Texture(m_context);
		m_dpethStencilReadOnly->CreateDepthStencilTexture(width, height, DXGI_FORMAT_R24G8_TYPELESS, true);

		// cbuffer ±îÁö...

		m_width		= width;
		m_height	= height;

		return true;
	}
	
	void GBuffer::Clear()
	{
		SAFE_DELETE(m_dpethStencilReadOnly);
		SAFE_DELETE(m_specPower);
		SAFE_DELETE(m_normal);
		SAFE_DELETE(m_colorSpecIntensity);
	}
	
	void GBuffer::PreRender()
	{
	}
}