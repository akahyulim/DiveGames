#pragma once
#include "Resource/Resource.h"
#include "GraphicsDefs.h"

namespace Dive
{
	class Context;
	class Graphics;
	class Viewport;

	// �ϴ� ��Ƽ ���ø� ������.
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

		// ���⿡�� �����Ѵٴ°� ���� �ָ��ϴ�.
		ID3D11Texture2D* GetTexture2D() const { return m_pTexture2D; }
		
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }

		void SetMipLevelsDirty();
		bool GetMipLevelsDirty() const { return m_bMipLevelsDirty; }
		void GenerateLevels();

		unsigned int GetRowPitchSize(int width) const;
		
		// SRGB�� ����?
		static DXGI_FORMAT GetSRVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
		static unsigned int CheckMaxMipmapCount(int width, int height);

		unsigned int GetViewportCount() const { return static_cast<unsigned int>(m_Viewports.size()); }
		Viewport* GetViewport(unsigned int index) const { return m_Viewports[index]; }
		void SetViewport(unsigned int index, Viewport* pViewport);

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

		bool m_bMipLevelsDirty;	// ���� ������ ���̰� �ֳ�...?


		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;
		// Array, Cube������ �߰��� �ʿ��� �� ����.

		// ���ͷ� ������ ��ŭ 2�� �̻��� ����ұ�?
		std::vector<Viewport*> m_Viewports;
	};
}