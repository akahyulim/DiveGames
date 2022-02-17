#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"

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

		// �Ϲ������δ� Backbuffer�� RenderTarget�� ResourceView���� ũ�Ⱑ ��������
		// ImGui�� Ȱ���� Editor ������ ũ�Ⱑ �޶��� �� �ִ�.
		void SetResolution(unsigned int width, unsigned int height);
		void SetTextures(unsigned int width, unsigned int height);

		ID3D11RenderTargetView* GetMainRenderTargetView();

		GraphicsDevice* GetGraphicsDevice() { return &m_GraphicsDevice; }

		unsigned int GetTextureWidth() const { return m_TextureWidth; }
		unsigned int GetTextureHeight() const { return m_TextureHeight; }

		// test
		Texture2D* GetSampleTexture() { return m_pSampleTex; }

	private:
	
	private:
		GraphicsDevice m_GraphicsDevice;

		unsigned int m_TextureWidth = 0;
		unsigned int m_TextureHeight = 0;

		// test resources
		Texture2D* m_pSampleTex = nullptr;
		Texture2D* m_pDepthStencilTex = nullptr;

		// test states
		ID3D11DepthStencilState* m_pDepthStencilState = nullptr;
	};
}