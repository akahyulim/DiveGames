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
		~GraphicsDevice() = default;

		void PresentBegin();
		void PresentEnd();

		void ResizeBuffers(unsigned int width, unsigned int height);
		void ResizeTarget(unsigned int width, unsigned int height);

		unsigned int GetResolutionWidth() const { return m_ResolutionWidth; }
		unsigned int GetResolutionHeight() const { return m_ResolutionHeight; }

		DXGI_FORMAT GetBackbufferFormat() const { return m_Format; }
		unsigned int GetBackbufferCount() const { return m_BackBufferCount; }

		bool GetVSyncEnabled() const { return m_bVSync; }
		void SetVSuncEnabled(bool use) { m_bVSync = use; }

		bool IsInitialized() const;

		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetImmediateContext() { return m_pImmediateContext; }

	private:
		void createBackbufferResources();
		void createDepthStencilView();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pBackBuffer;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

		bool m_bVSync = false;
		bool m_bFullScreen = false;
		unsigned int m_ResolutionWidth = 0;
		unsigned int m_ResolutionHeight = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int m_BackBufferCount = 2;
	};
}