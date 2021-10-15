#pragma once
#include "Src/Core/DiveEngine.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"

namespace Editor
{
	class Editor;

	class Widget
	{
	public:
		Widget(Editor* pEditor);
		virtual ~Widget() = default;

		void Tick();

		// menubar, toolbar, progressbar만 구현한다.
		virtual void TickAlways() {}
		// 일반적인 윈도우의 설정이다.
		virtual void TickVisible() {}
		virtual void OnShow() {}
		virtual void OnHide() {}
		virtual void OnPushStyleVar() {}

		void OnSetActiveScene();

		bool IsVisible() const { return m_bVisible; }
		void SetVisible(bool visible) { m_bVisible = visible; }
		float GetHeight() const { return m_Height; }
		std::string GetTitle() const { return m_Title; }
		ImGuiWindow* GetWindow() const { return m_pWindow; }

	protected:
		bool m_bWindow = true;
		bool m_bVisible = true;
		int m_Flags = ImGuiWindowFlags_NoCollapse;
		float m_Height = 0.0f;
		float m_Alpha = -1.0f;
		ImVec2 m_Position = ImVec2(-1.0f, -1.0f);
		ImVec2 m_Size = ImVec2(-1.0f, -1.0f);
		ImVec2 m_MaxSize = ImVec2(FLT_MAX, FLT_MAX);
		ImVec2 m_Padding = ImVec2(-1.0f, -1.0f);
		std::string m_Title = "Title";
		ImGuiWindow* m_pWindow = nullptr;

		Editor* m_pEditor = nullptr;
		dive::Scene* m_pScene = nullptr;

	private:
		unsigned char m_VarPushs = 0;
	};
}