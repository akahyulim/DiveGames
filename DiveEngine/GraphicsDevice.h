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

		unsigned int GetResolutionWidth() const { return mResolutionWidth; }
		unsigned int GetResolutionHeight() const { return mResolutionHeight; }

		DXGI_FORMAT GetBackbufferFormat() const { return mFormat; }
		unsigned int GetBackbufferCount() const { return mBackBufferCount; }

		bool GetVSyncEnabled() const { return mbVSync; }
		void SetVSuncEnabled(bool use) { mbVSync = use; }

		bool IsInitialized() const;

		ID3D11Device* GetDevice() { return mDevice.Get(); }
		ID3D11DeviceContext* GetImmediateContext() { return mImmediateContext.Get(); }

	private:
		void createBackbufferResources();
		void createDepthStencilView();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mBackBuffer;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;

		bool mbVSync = false;
		bool mbFullScreen = false;
		unsigned int mResolutionWidth = 0;
		unsigned int mResolutionHeight = 0;
		DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int mBackBufferCount = 2;
	};
}