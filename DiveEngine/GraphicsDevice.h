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

		// Create Resources


		// Bind

		void PresentBegin();
		void PresentEnd();

		void ResizeTarget(unsigned int width, unsigned int height);

		unsigned int GetResolutionWidth() const { return m_ResolutionWidth; }
		unsigned int GetResolutionHeight() const { return m_ResolutionHeight; }
		void SetResolution(unsigned int width, unsigned int height);

		DXGI_FORMAT GetFormat() const { return m_Format; }
		unsigned int GetBackbufferCount() const { return m_BackBufferCount; }

		bool GetVSyncEnabled() const { return m_bVSync; }
		void SetVSuncEnabled(bool use) { m_bVSync = use; }

		bool IsInitialized() const;

		// WickedEngine은 모든 Resource 생성과 Bind를 함수로 지원하므로
		// 아래의 장치 객체 리턴 함수는 존재하지 않는다.
		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetImmediateContext() { return m_pImmediateContext; }

		// WicekdEngine은 Backbuffer를 Texture로 리턴하는 함수가 있지만,
		// 아래의 RenderTargetView 리턴 함수는 없다.
		ID3D11RenderTargetView* GetRTV() { return m_pRenderTargetView; }

	private:
		void createBackbufferResources();

	private:
		IDXGISwapChain* m_pSwapChain = nullptr;
		ID3D11Device* m_pDevice = nullptr;
		ID3D11DeviceContext* m_pImmediateContext = nullptr;

		ID3D11Texture2D* m_pBackBuffer = nullptr;
		ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

		bool m_bVSync = false;
		bool m_bFullScreen = false;
		int m_ResolutionWidth = 0;
		int m_ResolutionHeight = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int m_BackBufferCount = 2;
	};
}