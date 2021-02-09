#include "DivePch.h"
#include "GBuffer.h"
#include "Texture2D.h"
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

		m_colorSpecIntensity	= new Texture2D(m_context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, "ColorSpecularIntensity");
		m_normal				= new Texture2D(m_context, width, height, DXGI_FORMAT_R11G11B10_FLOAT, "Normal");
		m_specPower				= new Texture2D(m_context, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, "SpecPower");

		// cbuffer ±îÁö...

		m_width		= width;
		m_height	= height;

		return true;
	}
	
	void GBuffer::Clear()
	{
		SAFE_DELETE(m_specPower);
		SAFE_DELETE(m_normal);
		SAFE_DELETE(m_colorSpecIntensity);
	}
	
	void GBuffer::PreRender()
	{
	}
}