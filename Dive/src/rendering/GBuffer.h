#pragma once

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class Texture2D;

	class GBuffer
	{
	public:
		GBuffer();
		~GBuffer();

		bool Resize(uint32_t width, uint32_t height);
		void Shutdown();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		DirectX::XMFLOAT2 GetSize() { return DirectX::XMFLOAT2{ static_cast<float>(m_Width), static_cast<float>(m_Height) }; }

		Texture2D* GetDiffuseTex() const { return m_pDiffuseTex; }
		Texture2D* GetNormalTex() const { return m_pNormalTex; }
		Texture2D* GetSpecularTex() const { return m_pSpecularTex; }
		Texture2D* GetDepthTex() const { return m_pDepthTex; }

	private:
		uint32_t m_Width;
		uint32_t m_Height;

		Texture2D* m_pDiffuseTex;
		Texture2D* m_pNormalTex;
		Texture2D* m_pSpecularTex;
		Texture2D* m_pDepthTex;
	};
}