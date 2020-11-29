#include "DivePch.h"
#include "Texture.h"
#include "RenderDevice.h"
#include "Rendering/Renderer.h"
#include "Core/Log.h"
#include "Core/SystemManager.h"


namespace Dive
{
	Texture::Texture(SystemManager* manager)
		: Resource(eResourceType::Texture)
	{
		m_renderDevice = manager->GetSystem<Renderer>()->GetRenderDevice();
	}

	Texture::~Texture()
	{

	}
}