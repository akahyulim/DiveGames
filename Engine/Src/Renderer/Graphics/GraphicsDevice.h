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

		bool CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Buffer* pBuffer);
		bool CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pData, ID3D11Texture2D** ppTexture2D);
		bool CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppShaderResourceView);
		bool CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRenderTargetView);
		bool CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView);

		// shader
		// sampler

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