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
	typedef uint8_t CommandList;
	static const CommandList COMMANDLIST_COUNT = 32;

	class GraphicsDevice
	{
	public:
		GraphicsDevice(HWND hWnd, bool fullScreen, bool debugLayer = false);
		~GraphicsDevice() = default;

		// resource 생성 함수
		// 직접 생성할 것이 아니라면 Device를 넘겨야 한다.

		CommandList BeginCommandList();

		void PresentBegin(CommandList cmd);
		void PresentEnd();

		void SetResolution(unsigned int width, unsigned int height);

		unsigned int GetResolutionWidth() const { return m_resolutionWidth; }
		unsigned int GetResolutionHeight() const { return m_resolutionHeight; }

		DXGI_FORMAT GetBackbufferFormat() const { return m_format; }
		unsigned int GetBackbufferCount() const { return m_backBufferCount; }

		bool GetVSyncEnabled() const { return m_bVSync; }
		void SetVSuncEnabled(bool use) { m_bVSync = use; }

		// resource bind 함수
		// 역시 직접 묶을게 아니라면 deferredContext를 넘겨야 한다.
		// 따라서 스레드에 민감하다.
		void BindViewports(unsigned int count, const D3D11_VIEWPORT* viewports, CommandList cmd);

	private:
		void createBackbufferResources();
		void submitCommandLists();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_immediateContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContexts[COMMANDLIST_COUNT];
		Microsoft::WRL::ComPtr<ID3D11CommandList> m_commandLists[COMMANDLIST_COUNT];

		bool m_bVSync = false;
		bool m_bFullScreen = false;
		unsigned int m_resolutionWidth = 0;
		unsigned int m_resolutionHeight = 0;
		DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		unsigned int m_backBufferCount = 2;

		std::atomic<CommandList> m_commandCount{ 0 };

		// resources
		// 이전에 사용한 리소스들이다. 각 CommandList별로 존재한다.
		// State들도 당연히 포함되는데, 직접 만든 객체로 할가...
		D3D11_PRIMITIVE_TOPOLOGY m_prevPrimitiveTopologies[COMMANDLIST_COUNT];
		
		ID3D11VertexShader* m_prevVertexShaders[COMMANDLIST_COUNT];
		ID3D11PixelShader* m_prevPixelShaders[COMMANDLIST_COUNT];
	};
}