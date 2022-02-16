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

		// 객체들을 직접 리턴하는게 맞나 싶긴한데
		// 결국 만들어 지는건 단순 랩핑 함수들이다...
		// 그런데 Hazel은 RendererAPI라고 따로 뽑아놨다.
		// 물론 멀티 플랫폼을 위한 인터페이스일 수 있다.

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