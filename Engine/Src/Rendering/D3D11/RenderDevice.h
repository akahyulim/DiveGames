#pragma once
#include "RenderDeviceEnums.h"

namespace Dive
{
	class RenderDevice
	{
	public:
		RenderDevice(HWND hWnd, int width, int height, bool windowed);
		~RenderDevice();

		bool Present();
		void ResizeBuffer(int width, int height);

		bool IsInitialized()							const { return m_bInitialized; }
		ID3D11Device* GetD3dDevice()					const { return m_D3dDevice; }
		ID3D11DeviceContext* GetImmediateContext()		const { return m_ImmediateContext; }
		IDXGISwapChain* GetSwapChain()					const { return m_SwapChain; }
		ID3D11RenderTargetView* GetRenderTargetView()	const { return m_RenderTargetView; }

	private:
		ID3D11Device* m_D3dDevice					= nullptr;;
		ID3D11DeviceContext* m_ImmediateContext		= nullptr;
		IDXGISwapChain* m_SwapChain					= nullptr;
		ID3D11RenderTargetView* m_RenderTargetView	= nullptr;	// 이게 이 곳에서 Texture2D가 아닌 방법으로 생성되었다는 것은 backbuffer이기 때문?

		// window datas
		HWND m_hWnd;
		int m_Width			= 800;
		int m_Height		= 600;
		bool m_bWindowed	= true;

		bool m_bInitialized	= false;


		bool m_bVSync		= false;
	};
}

