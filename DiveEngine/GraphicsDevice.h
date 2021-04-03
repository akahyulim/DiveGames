#pragma once
#include "GraphicsInclude.h"
#include "GraphicsEnums.h"
#include "CommonInclude.h"
#include <Windows.h>

namespace Dive
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer = false);
		~GraphicsDevice() = default;

		void PresentBegin();
		void PresentEnd();

		void SetResolution(unsigned int width, unsigned int height);

		unsigned int GetResolutionWidth() const { return m_resolutionWidth; }
		unsigned int GetResolutionHeight() const { return m_resolutionHeight; }

		DXGI_FORMAT GetBackbufferFormat() const { return m_format; }
		unsigned int GetBackbufferCount() const { return m_backBufferCount; }

		bool GetVSyncEnabled() const { return m_bVSync; }
		void SetVSuncEnabled(bool use) { m_bVSync = use; }

		bool IsInitialized() const;

		ID3D11Device* GetDevice() { return m_pDevice.Get(); }
		ID3D11DeviceContext* GetImmediateContext() { return m_pImmediateContext.Get(); }

	private:
		void createBackbufferResources();
		void createDepthStencilView();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDSBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;

		bool m_bVSync = false;
		bool m_bFullScreen = false;
		unsigned int m_resolutionWidth = 0;
		unsigned int m_resolutionHeight = 0;
		DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int m_backBufferCount = 2;
	};
}