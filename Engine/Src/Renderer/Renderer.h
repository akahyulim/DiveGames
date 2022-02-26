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

		// �Ϲ������δ� Backbuffer�� RenderTarget�� ResourceView���� ũ�Ⱑ ��������
		// ImGui�� Ȱ���� Editor ������ ũ�Ⱑ �޶��� �� �ִ�.
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