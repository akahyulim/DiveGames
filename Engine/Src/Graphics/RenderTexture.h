#pragma once
#include "Texture.h"

namespace Dive
{
	struct RenderTextureDescriptor
	{
		uint32_t width;
		uint32_t height;
		uint32_t mipCount;

		DXGI_FORMAT format;

		// �ؽ��� ��Ǽ�
		// �迭 ������
	};

	class RenderTexture : public Texture
	{
		DV_CLASS(RenderTexture, Texture)

	public:
		RenderTexture();	// �����
		//RenderTexture(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipMap = true);
		//RenderTexture(uint32_t width, uint32_t height, DXGI_FORMAT format, bool readOnly = false);
		//RenderTexture(const RenderTextureDescriptor& desc);
		~RenderTexture() override;	

		// ����Ƽ�� �����ڿ� ���� Ȥ�� desc�� �����ϰ�
		// Create �Լ��� �����Ѵ�.
		bool CreateRenderTargetView(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool useMipMap = true);
		bool CreateDepthStencilView(uint32_t width, uint32_t height, DXGI_FORMAT format, bool readOnly = false);
		//bool Create();
		void Release();

		ID3D11ShaderResourceView* GetShaderResourceView() override { return m_pShaderResourceView; }
		
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView; }
		ID3D11DepthStencilView* GetDepthStencilViewReadOnly() const { return m_pDepthStencilViewReadOnly; }

	private:
		// ����Ƽ�� �ᱹ render target�� depth buffer�� �ٸ� texture, shader resource view�� �����Ѵ�.
		ID3D11Texture2D* m_pTexture2D;
		ID3D11ShaderResourceView* m_pShaderResourceView;
		ID3D11RenderTargetView* m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11DepthStencilView* m_pDepthStencilViewReadOnly;

		// descriptor
		// texture demansion
		// array size
	};
}