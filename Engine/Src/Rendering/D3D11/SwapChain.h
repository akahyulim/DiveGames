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
		// ���� ī��Ʈ
		// ����
		// ����̽�
		int m_Width;
		int m_Height;

		
		IDXGISwapChain* m_SwapChain;
		ID3D11RenderTargetView* m_RenderTargetView;

		// RenderTargetView ������ �ʿ��ϴ�. ��, Resize���� �ʿ��ϴ�?
		// shared_ptr??
		ID3D11Device* m_Device;
	};
}

