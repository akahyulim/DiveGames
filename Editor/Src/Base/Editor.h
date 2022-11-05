#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class MenuBarPanel;
	class ScenePanel;
	class HierarchyPanel;
	class InspectorPanel;
	class AssetPanel;

	class Editor : public Dive::Application
	{
	public:
		explicit Editor(Dive::Context* pContext);
		~Editor() override;

		void Setup() override;
		void Start() override;
		void Stop() override;

		void OnBeginRender(const Dive::Event& e);
		void OnEndRender(const Dive::Event& e);
		void OnWindowEvent(const Dive::Event& e);

		MenuBarPanel* GetMenuBarPanel() const { return m_pMenuBar.get(); }
		ScenePanel* GetScenePanel() const { return m_pScene.get(); }
		HierarchyPanel* GetHierarchyPanel() const { return m_pHierarchy.get(); }
		InspectorPanel* GetInspectorPanel() const { return m_pInspector.get(); }
		AssetPanel* GetAssetPanel() const { return m_pAsset.get(); }
	
	private:
		void drawPanels();

	private:
		std::unique_ptr<MenuBarPanel> m_pMenuBar;
		std::unique_ptr<ScenePanel> m_pScene;
		std::unique_ptr<HierarchyPanel> m_pHierarchy;
		std::unique_ptr<InspectorPanel> m_pInspector;
		std::unique_ptr<AssetPanel> m_pAsset;
	};
}