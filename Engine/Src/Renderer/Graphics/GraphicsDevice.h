#pragma once

namespace Dive
{
	struct WindowData;

	class GraphicsDevice
	{
	public:
		GraphicsDevice() = default;
		~GraphicsDevice() = default;

		void Initialize(const WindowData* pData);
		void Shutdown();

		void CreateMainRenderTargetView();
		void CleanupMainRenderTargetView();

		void ResizeBackBuffer(unsigned int width, unsigned int height);

		// create resources
		bool CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer** ppBuffer);
		bool CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Texture2D** ppTexture2D);
		bool CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppShaderResourceView);
		bool CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRenderTargetView);
		bool CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView);
		bool CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDesc, ID3D11DepthStencilState** ppDepthStencilState);
		bool CreateSamplerState(const D3D11_SAMPLER_DESC* pDesc, ID3D11SamplerState** ppSamplerState);
		bool CreateBlendState(const D3D11_BLEND_DESC* pDesc, ID3D11BlendState** ppBlendState);
		bool CreateRasterizerState(const D3D11_RASTERIZER_DESC* pDesc, ID3D11RasterizerState** ppRasterizerState);
		bool CreateVertexShader(const std::string& path, const D3D11_INPUT_ELEMENT_DESC* pDesc, unsigned int numElements, ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppInputLayout);
		bool CreatePixelShader(const std::string& path, ID3D11PixelShader** ppPixelShader);
		bool CreateShader(const std::string& path, const D3D11_INPUT_ELEMENT_DESC* pDesc, unsigned int numElements, ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppInputLayout, ID3D11PixelShader** ppPixelShader);

		// map / unmap
		bool Map(ID3D11Resource* pResource, unsigned int subresource, D3D11_MAP mapType, unsigned int mapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedSubresource);
		void Unmap(ID3D11Resource* pResource, unsigned int subresource);

		// begin / end


		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetImmediateContext() { return m_pImmediateContext; }

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		ID3D11RenderTargetView* GetMainRenderTargetView() { return m_pMainRenderTargetView; }

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }
		
		bool IsVSync() const { return m_bVSync; }

		bool IsFullScreen() const { return m_bFullScreen; }

	private:
	
	private:
		ID3D11Device* m_pDevice								= nullptr;
		ID3D11DeviceContext* m_pImmediateContext			= nullptr;
		IDXGISwapChain* m_pSwapChain						= nullptr;
		ID3D11RenderTargetView* m_pMainRenderTargetView		= nullptr;

		HWND m_hWnd						= 0;
		unsigned int m_BackbufferCount	= 2;
		unsigned int m_Width			= 0;
		unsigned int m_Height			= 0;
		bool m_bVSync					= false;
		bool m_bFullScreen				= false;
		DXGI_FORMAT m_Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
		
	};
}