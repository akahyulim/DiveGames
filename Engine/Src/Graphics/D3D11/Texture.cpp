#include "DivePch.h"
#include "Texture.h"
#include "RenderDevice.h"
#include "Graphics/Graphics.h"
#include "Graphics/Renderer.h"
#include "Core/Log.h"
#include "Core/Context.h"


namespace Dive
{
	Texture::Texture(Context* context)
		: Resource(context, eResourceType::Texture)
	{
		//m_renderDevice = GetSubsystem<Graphics>()->Get
	}

	Texture::~Texture()
	{

	}
}