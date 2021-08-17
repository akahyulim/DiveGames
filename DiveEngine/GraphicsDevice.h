#pragma once
#include "GraphicsInclude.h"
#include "GraphicsEnums.h"
#include <Windows.h>

namespace dive
{
	class GraphicsDevice
	{
	public:
		GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer = false);
		~GraphicsDevice();

		void PresentBegin();
		void PresentEnd();

		void ResizeBuffers(unsigned int width, unsigned int height);
		void ResizeTarget(unsigned int width, unsigned int height);

		unsigned int GetResolutionWidth() const { return m_Width; }
		unsigned int GetResolutionHeight() const { return m_Height; }

		DXGI_FORMAT GetFormat() const { return m_Format; }
		unsigned int GetBackbufferCount() const { return m_BackBufferCount; }

		bool GetVSyncEnabled() const { return m_bVSync; }
		void SetVSuncEnabled(bool use) { m_bVSync = use; }

		bool IsInitialized() const;

		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetImmediateContext() { return m_pImmediateContext; }

		ID3D11RenderTargetView* GetRTV() { return m_pRenderTargetView; }
		ID3D11DepthStencilView* GetDSV() { return m_pDepthStencilView; }

	private:
		void createBackbufferResources();

	private:
		IDXGISwapChain* m_pSwapChain = nullptr;
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
		ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
		ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

		bool m_bVSync = false;
		bool m_bFullScreen = false;
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int m_BackBufferCount = 2;
	};
}