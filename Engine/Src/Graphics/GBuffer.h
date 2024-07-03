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
		void Release();

		RenderTexture* GetDiffuseTex() const { return m_pDiffuseTex; }
		RenderTexture* GetNormalTex() const { return m_pNormalTex; }
		RenderTexture* GetSpecularTex() const { return m_pSpecularTex; }
		RenderTexture* GetDepthTex() const { return m_pDepthTex; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		DirectX::XMFLOAT2 GetSize() { return DirectX::XMFLOAT2{ static_cast<float>(m_Width), static_cast<float>(m_Height) }; }

	private:
		Graphics* m_pGraphics;

		RenderTexture* m_pDiffuseTex;
		RenderTexture* m_pNormalTex;
		RenderTexture* m_pSpecularTex;
		RenderTexture* m_pDepthTex;

		uint32_t m_Width;
		uint32_t m_Height;
	};
}