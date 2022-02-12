#include "divepch.h"
#include "Renderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Base/Engine.h"

namespace Dive
{
	void Renderer::Initialize(const WindowData* pData)
	{
		m_GraphicsDevice.Initialize(pData);
	}

	void Renderer::Shutdow()
	{
		m_GraphicsDevice.Shutdown();
	}
}