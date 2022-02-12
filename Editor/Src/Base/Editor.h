#pragma once
#include "DiveEngine.h"
#include <Windows.h>
#include <string>

// panels
class MenuBarPanel;
class ScenePanel;

class Editor
{
public:
	Editor(HINSTANCE hInstance, const std::string& title = "DiveEditor");
	~Editor();

	void Run();
	void Close() { m_bDone = true; }
	void Shutdown();

	void CreateRenderTarget();
	void CleanupRenderTarget();

	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	ID3D11Device* GetD3dDevice() { return m_pD3dDevice; }
	ID3D11DeviceContext* GetD3dDeviceContext() { return m_pD3dDeviceContext; }
	ID3D11RenderTargetView* GetRnederTargetView() { return m_pRenderTargetView; }

private:
	void createWindow(HINSTANCE hInstance);

	void createDeviceD3D();
	void cleanupDeviceD3D();

	void intializeImGui();
	void setDarkThemeColors();
	void loadResources();

	void drawPanels();

private:
	// window
	HINSTANCE m_hInstance	= 0;
	HWND m_hWnd				= 0;
	std::string m_Title;
	unsigned int m_Width	= 0;
	unsigned int m_Height	= 0;
	bool m_bVSync			= false;
	bool m_bFullScreen		= false;


	// d3d11 device
	IDXGISwapChain* m_pSwapChain				= nullptr;
	ID3D11Device* m_pD3dDevice					= nullptr;
	ID3D11DeviceContext* m_pD3dDeviceContext	= nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	// panels
	MenuBarPanel* m_pMenuBarPanel = nullptr;
	ScenePanel* m_pScenePanel = nullptr;

	bool m_bDone = false;
};