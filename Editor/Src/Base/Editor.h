#pragma once
#include "DiveEngine.h"
#include "Panels/MenubarPanel.h"

namespace Dive
{
	class AppWindow;

	struct WindowProps
	{
		std::string Title	= "DiveEditor";
		unsigned int Width	= 1280;
		unsigned int Height = 780;
		bool Maximize		= false;
	};

	class Editor : public Engine
	{
	public:
		Editor(HINSTANCE hInstance);
		~Editor();

		void Initialize() override;

		void Run();

		const WindowProps& GetWindowProps() const { return m_WindowProps; }

	private:
		// d3d11 device
		void createDeviceD3D();
		void cleanupDeviceD3D();
		void createRenderTarget();
		void cleanupRenderTarget();

		// imGui
		void renderPanels();
		void beginFrame();
		void endFrame();

	private:
		// window
		AppWindow* m_pAppWnd = nullptr;
		WindowProps m_WindowProps;

		// d3d11 device
		ID3D11Device* m_pD3dDevice						= nullptr;
		ID3D11DeviceContext* m_pD3dDeviceContext		= nullptr;
		IDXGISwapChain* m_pSwapChain					= nullptr;
		ID3D11RenderTargetView* m_pMainRenderTargetView = nullptr;

		// panels
	};
}