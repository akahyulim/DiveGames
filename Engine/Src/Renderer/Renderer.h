#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"

namespace Dive
{
	struct WindowData;

	class Renderer
	{
	public:
		static void Initialize(const WindowData* pData);
		static void Shutdown();

		static void BeginScene();
		static void EndScene();

		//static void SetViewport(float width, float height);

		// 일반적으로는 Backbuffer의 RenderTarget과 ResourceView들의 크기가 같겠지만
		// ImGui를 활용한 Editor 때문에 크기가 달라질 수 있다.
		static void SetResolution(unsigned int width, unsigned int height);
		static void SetTextures(unsigned int width, unsigned int height);

		static ID3D11RenderTargetView* GetMainRenderTargetView();

		static GraphicsDevice* GetGraphicsDevice() { return &m_GraphicsDevice; }

		static unsigned int GetTextureWidth() { return m_TextureWidth; }
		static unsigned int GetTextureHeight() { return m_TextureHeight; }

		// test
		static Texture2D* GetSampleTexture() { return m_pSampleTex; }

	private:
	
	private:
		static GraphicsDevice m_GraphicsDevice;

		static unsigned int m_TextureWidth;
		static unsigned int m_TextureHeight;

		// test resources
		static Texture2D* m_pSampleTex;
		static Texture2D* m_pDepthStencilTex;

		// test states
		static ID3D11DepthStencilState* m_pDepthStencilState;
	};
}