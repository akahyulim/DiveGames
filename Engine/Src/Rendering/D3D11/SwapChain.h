#pragma once

namespace Dive
{
	class RenderDevice;

	class SwapChain
	{
	public:
		SwapChain(const std::shared_ptr<RenderDevice>& device, HWND hWnd, int width, int height);
		~SwapChain();

		bool Present();

		void ResizeBuffer(int width, int height);

		IDXGISwapChain* GetSwapChain() const { return m_SwapChain; }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }

	private:
		HWND m_hWnd;
		bool m_bWindowed;
		// 버퍼 카운트
		// 포멧
		// 디바이스
		int m_Width;
		int m_Height;

		
		IDXGISwapChain* m_SwapChain;
		ID3D11RenderTargetView* m_RenderTargetView;

		// RenderTargetView 생성에 필요하다. 즉, Resize에도 필요하다?
		std::shared_ptr<RenderDevice> m_RenderDevice;
	};
}

