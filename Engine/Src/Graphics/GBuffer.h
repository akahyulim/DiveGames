#pragma once

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class Graphics;
	class RenderTexture;

	class GBuffer
	{
	public:
		GBuffer();
		~GBuffer();

		bool Initialize(uint32_t width, uint32_t height);

		void BindToRenderTargets();
		void BindToResourceViews();

	private:
	private:
		Graphics* m_pGraphics;

		RenderTexture* m_pAlbedoTex;
		RenderTexture* m_pNormalTex;
		RenderTexture* m_pSpecularTex;
		RenderTexture* m_pDepthTex;
	};
}