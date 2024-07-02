#include "DivePch.h"
#include "Resource.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Renderer/Mesh.h"

namespace Dive
{
	Resource::Resource()
		: m_NameHash(0)
	{
	}
	
	void Resource::SetName(const std::string& name)
	{
		m_Name = name;
		m_NameHash = std::hash<std::string>{}(name);
	}
}