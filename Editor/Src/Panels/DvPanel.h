#pragma once
#include <string>
#include "Base/DvEditor.h"
#include "DiveEngine.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"
#include "imgui-docking/imgui_stdlib.h"

namespace Editor
{
	class DvPanel
	{
	public:
		DvPanel(DvEditor* pEditor, const std::string& title = "untitled");
		virtual ~DvPanel() = default;

		void Tick();

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		bool IsVisible() const { return m_bVisible; }
		void SetVisible(bool visible) { m_bVisible = visible; }

		Dive::Scene* GetActiveScene() {
			return nullptr; // m_pActiveScene;
	}
		void SetActiveScene(Dive::Scene* pScene) { 
			//m_pActiveScene = pScene; 
		}

	protected:
		// 항상 그려져야 하는 것(ex. MenuBar)
		virtual void renderAlways() {}
		// 창 형태의 Panel
		virtual void renderWindow() {}

	protected:
		std::string m_Title;
		float m_Width = 0.0f;
		float m_Height = 0.0f;
		bool m_bVisible = true;
		bool m_bWindow = true;

		DvEditor* m_pEditor = nullptr;

		//Dive::Scene* m_pActiveScene = nullptr;
	};
}