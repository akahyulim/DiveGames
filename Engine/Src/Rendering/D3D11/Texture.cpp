#include "DivePch.h"
#include "Texture.h"
#include "RenderDevice.h"
#include "Rendering/Renderer.h"
#include "Core/Log.h"


namespace Dive
{
	Texture::Texture(ID3D11Device* device)
		: IResource(eResourceType::Texture), m_D3dDevice(device)
	{
	}

	Texture::~Texture()
	{

	}
}