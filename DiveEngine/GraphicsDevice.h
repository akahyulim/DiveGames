#pragma once
#include "CommonInclude.h"
#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <wrl/client.h>
#include <Windows.h>

namespace Dive
{
	// device, contex로 할 수 있는 것들을 함수화한다.
	class GraphicsDevice
	{
	public:
		GraphicsDevice(HWND hWnd, bool fullScreen);
		~GraphicsDevice() = default;

	private:
		GraphicsDevice(const GraphicsDevice&)				= delete;
		GraphicsDevice& operator=(const GraphicsDevice&)	= delete;

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
	};
}