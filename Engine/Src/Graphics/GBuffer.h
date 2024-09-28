#pragma once

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class Graphics;
	class RenderTexture;
	class DvTexture2D;

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

		DvTexture2D* GetDiffuseTexDv() const { return m_pDiffuseTexDv; }
		DvTexture2D* GetNormalTexDv() const { return m_pNormalTexDv; }
		DvTexture2D* GetSpecularTexDv() const { return m_pSpecularTexDv; }
		DvTexture2D* GetDepthTexDv() const { return m_pDepthTexDv; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		DirectX::XMFLOAT2 GetSize() { return DirectX::XMFLOAT2{ static_cast<float>(m_Width), static_cast<float>(m_Height) }; }

	private:
		Graphics* m_pGraphics;

		RenderTexture* m_pDiffuseTex;
		RenderTexture* m_pNormalTex;
		RenderTexture* m_pSpecularTex;
		RenderTexture* m_pDepthTex;


		DvTexture2D* m_pDiffuseTexDv;
		DvTexture2D* m_pNormalTexDv;
		DvTexture2D* m_pSpecularTexDv;
		DvTexture2D* m_pDepthTexDv;

		uint32_t m_Width;
		uint32_t m_Height;
	};
}