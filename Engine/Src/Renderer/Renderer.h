#pragma once
#include "Graphics/GraphicsDevice.h"

namespace Dive
{
	struct WindowData;

	class Renderer
	{
	public:
		void Initialize(const WindowData* pData);
		void Shutdow();

	private:
	
	private:
		GraphicsDevice m_GraphicsDevice;
	};
}