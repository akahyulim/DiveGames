#pragma once
#include "CommonInclude.h"
#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <wrl/client.h>
#include <Windows.h>
#include <atomic>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

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

		// resource bind 함수
		// 이것두 괜히 랩핑 형태가 될 수 있다.
		void BindViewports(unsigned int count, const D3D11_VIEWPORT* viewports);

		// 이름이 좀 에바다. 
		ID3D11Device* GetDevice() { return m_device.Get(); }
		ID3D11DeviceContext* GetImmediateContext() { return m_immediateContext.Get(); }

	private:
		void createBackbufferResources();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;

		bool m_bVSync = false;
		bool m_bFullScreen = false;
		unsigned int m_resolutionWidth = 0;
		unsigned int m_resolutionHeight = 0;
		DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int m_backBufferCount = 2;
	};
}