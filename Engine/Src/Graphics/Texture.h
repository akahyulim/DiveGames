#pragma once
#include "Resource/Resource.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class Graphics;

	// 일단 멀티 샘플링 미지원.
	class Texture : public Resource
	{
		DIVE_OBJECT(Texture, Resource)

	public:
		explicit Texture(Context* pContext);
		~Texture() override;

		DXGI_FORMAT GetFormat() const { return m_Format; }

		eTextureUsage GetUsage() const { return m_Usage; }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		int GetDepth() const { return m_Depth; }

		unsigned int GetMipmapCount() const { return m_MipmapCount; }
		
		bool GetEnableMipmap() const { return m_bEnableMipmap; }
		void SetEnableMipmap(bool enable) { m_bEnableMipmap = enable; }

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }

		void GenerateLevels();

		unsigned int GetRowPitchSize(int width) const;
		
		// SRGB는 뭘까?
		static DXGI_FORMAT GetSRVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
		static unsigned int CheckMaxMipmapCount(int width, int height);

	private:

	protected:
		Graphics* m_pGraphics;
		DXGI_FORMAT m_Format;
		eTextureUsage m_Usage;
		int m_Width;
		int m_Height;
		int m_Depth;
		unsigned int m_MipmapCount;
		bool m_bEnableMipmap;


		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;
		// Array, Cube용으로 추가가 필요할 것 같다.
	};
}