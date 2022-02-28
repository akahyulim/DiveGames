#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"

namespace Dive
{
	struct WindowData;

	// ���� GraphicsDeivce�� �ʱ�ȭ �� ������ �ϰ�
	// �������� Base Resource ���� �� ������ �����ؾ� �� �� ����.
	class Renderer
	{
	public:
		static void Initialize(const WindowData* pData);
		static void Shutdown();

		// �ᱹ �����Լ���. Graphics���� ó���ϴ� ���� �´� ��...
		static void BeginScene();
		static void EndScene();

		//static void SetViewport(float width, float height);

		// �Ϲ������δ� Backbuffer�� RenderTarget�� ResourceView���� ũ�Ⱑ ��������
		// ImGui�� Ȱ���� Editor ������ ũ�Ⱑ �޶��� �� �ִ�.
		static void SetResolution(unsigned int width, unsigned int height);
		// �̸��� �ٲٰ� �ʹ�. ������ createTextures�� �ϱ� �����̴�.
		static void SetTextures(unsigned int width, unsigned int height);

		static GraphicsDevice& GetGraphicsDevice() { return m_GraphicsDevice; }

		static unsigned int GetTextureWidth() { return m_TextureWidth; }
		static unsigned int GetTextureHeight() { return m_TextureHeight; }

		// test ============================================================
		static Texture2D* GetSampleTexture() { return m_pSampleTex; }

	private:
		static void createRenderTargets();
		static void createSamplers();
		static void createDepthStencilStates();
		static void createRasterizerStates();
		static void createShaders();
	
	private:
		static GraphicsDevice m_GraphicsDevice;

		static unsigned int m_TextureWidth;
		static unsigned int m_TextureHeight;

		// render targets

		// samplers

		// depth stencil states

		// rasterizer states

		// shaders

		// test resources =================
		static Texture2D* m_pSampleTex;
		static Texture2D* m_pDepthStencilTex;
		static ID3D11DepthStencilState* m_pDepthStencilState;
	};
}