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

		void Tick();

		void BeginScene();
		void EndScene();

		void SetViewport(float width, float height);
		void SetResolution(unsigned int width, unsigned int height);

		ID3D11RenderTargetView* GetMainRenderTargetView();

	private:
	
	private:
		GraphicsDevice m_GraphicsDevice;

		// resources
	};
}