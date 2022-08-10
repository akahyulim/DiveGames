#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class DvMenuBarPanel;
	class DvScenePanel;
	class DvHierarchyPanel;
	class DvInspectorPanel;
	class DvAssetPanel;

	class DvEditor : public Dive::Application
	{
	public:
		DvEditor(Dive::DvContext* pContext);
		~DvEditor();

		void Setup() override;
		void Start() override;
		void Stop() override;

		void OnBeginRender(const Dive::DvEvent& e);
		void OnEndRender(const Dive::DvEvent& e);
		void OnWindowEvent(const Dive::DvEvent& e);

	private:
		void drawPanels();

	private:
		DvMenuBarPanel* m_pMenuBar = nullptr;
		DvScenePanel* m_pScene = nullptr;
		DvHierarchyPanel* m_pHierarchy = nullptr;
		DvInspectorPanel* m_pInspector = nullptr;
		DvAssetPanel* m_pAsset = nullptr;
	};
}