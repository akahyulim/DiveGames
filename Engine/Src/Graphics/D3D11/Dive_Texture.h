#pragma once
#include "Resource/Resource.h"


namespace Dive
{
	class Dive_Texture : public Resource
	{
	public:
		Dive_Texture(Context* context);
		~Dive_Texture();

		bool SaveToFile(const std::string& filepath) override;
		bool LoadFromFile(const std::string& filepath) override;

		bool CreateRenderTexture(unsigned int width, unsigned height, DXGI_FORMAT format, bool generateMipmaps = false);
		bool CreateDepthStencilTexture(unsigned int width, unsigned height, DXGI_FORMAT format, bool readOnly = false);
		bool CreateCubeTexture(unsigned int width, unsigned height, DXGI_FORMAT format);

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView; }
		ID3D11DepthStencilView* GetDepthStencilView() const		{ return m_depthStencilView; }
		ID3D11RenderTargetView* GetRenderTargetView() const		{ return m_renderTargetView; }

		unsigned int GetWidth() const { return m_width; }
		unsigned int GetHeight() const { return m_height; }
		D3D11_VIEWPORT GetViewport() const { return m_viewport; }

		void PrintTextureInfo();

	private:
		bool createTexture2D(bool generateMipmaps = false);
		bool createShaderResourceView();
		bool createRenderTargetView();
		bool createDepthStencilView(bool readOnly = false);

		DXGI_FORMAT getShaderResourceViewFormat();
		DXGI_FORMAT getDepthStencilViewFormat();

		void setViewport();

	private:
		ID3D11Device* m_device;
		ID3D11ShaderResourceView* m_shaderResourceView;
		ID3D11DepthStencilView* m_depthStencilView;
		ID3D11RenderTargetView* m_renderTargetView;
		ID3D11Texture2D* m_texture;

		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_arraySize;
		unsigned int m_mipLevels;
		unsigned int m_bindFlags;

		DXGI_FORMAT m_format;

		std::vector<std::vector<std::byte>> m_data;

		D3D11_VIEWPORT m_viewport;

		std::array<ID3D11DepthStencilView*, 6> m_depthStencilViews;
		std::array<ID3D11RenderTargetView*, 6> m_renderTargetViews;
	};
}