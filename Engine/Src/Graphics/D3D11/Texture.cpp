#include "DivePch.h"
#include "Texture.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Core/Log.h"
#include "Core/Context.h"


namespace Dive
{
	Texture::Texture(Context* context)
		: Resource(context)
	{
		m_resourceType = eResourceType::Texture;
		m_renderDevice = GetSubsystem<Graphics>()->GetRHIDevice();
	}

	Texture::~Texture()
	{
		// data clear
	}

	void Texture::setViewport(unsigned int width, unsigned int height)
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width	= static_cast<float>(width);
		m_viewport.Height	= static_cast<float>(height);
		// 하드코딩이다...
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}
}