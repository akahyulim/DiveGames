#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Texture.h"

namespace Dive
{
	struct WindowData;

	// 여긴 GraphicsDeivce의 초기화 및 관리만 하고
	// 나머지는 Base Resource 생성 및 관리로 제한해야 할 것 같다.
	class Renderer
	{
	public:
		static void Initialize(const WindowData* pData);
		static void Shutdown();

		// 결국 랩핑함수다. Graphics에서 처리하는 것이 맞는 듯...
		static void BeginScene();
		static void EndScene();

		//static void SetViewport(float width, float height);

		// 일반적으로는 Backbuffer의 RenderTarget과 ResourceView들의 크기가 같겠지만
		// ImGui를 활용한 Editor 때문에 크기가 달라질 수 있다.
		static void SetResolution(unsigned int width, unsigned int height);
		// 이름을 바꾸고 싶다. 생성은 createTextures가 하기 때문이다.
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