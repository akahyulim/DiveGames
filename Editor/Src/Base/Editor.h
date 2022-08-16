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
		Editor(Dive::Context* pContext);
		~Editor();

		void Setup() override;
		void Start() override;
		void Stop() override;

		void OnBeginRender(const Dive::Event& e);
		void OnEndRender(const Dive::Event& e);
		void OnWindowEvent(const Dive::Event& e);

	private:
		void drawPanels();

	private:
		MenuBarPanel* m_pMenuBar;
		ScenePanel* m_pScene;
		HierarchyPanel* m_pHierarchy;
		InspectorPanel* m_pInspector;
		AssetPanel* m_pAsset;
	};
}