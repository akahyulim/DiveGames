#pragma once
#include "DiveEngine.h"
#include "SceneViewCamera.h"
#include "EditorRenderPath.h"
#include <Windows.h>
#include <string>
#include <vector>

// panels
class Panel;
class MenuBarPanel;
class ScenePanel;
class HierarchyPanel;
class InspectorPanel;
class AssetPanel;

class Editor
{
public:
	Editor(HINSTANCE hInstance, const std::string& title = "DiveEditor");
	~Editor();

	void OnWindowMessage(const Dive::WindowData& data);

	void Run();
	void Close() { m_bDone = true; }
	void Shutdown();

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;

	// 꼭! 좀 더 직관적인 방법을 찾자!
	MenuBarPanel* GetMenuBar() { return m_pMenuBar; }
	ScenePanel* GetScene() { return m_pScene; }
	HierarchyPanel* GetHierarchy() { return m_pHierarchy; }
	InspectorPanel* GetInspector() { return m_pInspector; }
	AssetPanel* GetAsset() { return m_pAsset; }

private:
	// editor's window
	void createWindow(HINSTANCE hInstance);

	// imgui
	void intializeImGui();
	void setDarkThemeColors();
	void loadResources();
	void drawPanels();

private:
	// window
	std::string m_Title;
	HINSTANCE m_hInstance	= 0;
	HWND m_hWnd				= 0;
	unsigned int m_Width	= 0;
	unsigned int m_Height	= 0;
	bool m_bVSync			= false;
	bool m_bFullScreen		= false;

	// panels
	//std::vector<Panel*> m_Panels;
	MenuBarPanel* m_pMenuBar		= nullptr;
	ScenePanel* m_pScene			= nullptr;
	HierarchyPanel* m_pHierarchy	= nullptr;
	InspectorPanel* m_pInspector	= nullptr;
	AssetPanel* m_pAsset			= nullptr;

	bool m_bDone = false;

	enum class eSceneMode
	{
		Editor = 0,
		Play
	};

	eSceneMode m_SceneMode = eSceneMode::Editor;

	SceneViewCamera m_SceneViewCamera;
	EditorRenderPath m_EditorRenderPath;
};