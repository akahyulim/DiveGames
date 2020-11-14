#pragma once

namespace Dive
{
	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		void ResizeResolution(int width, int height);

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
		// shared_ptr??
		ID3D11Device* m_Device;
	};
}

