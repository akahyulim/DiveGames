#include "DivePch.h"
#include "Texture.h"
#include "RenderDevice.h"
#include "Graphics/Renderer.h"
#include "Core/Log.h"
#include "Core/Dive_Context.h"


namespace Dive
{
	Texture::Texture(Dive_Context* context)
		: Resource(eResourceType::Texture)
	{
		m_renderDevice = context->GetSubsystem<Renderer>()->GetRenderDevice();
	}

	Texture::~Texture()
	{

	}
}