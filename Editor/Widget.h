#pragma once
#include "DiveEngine.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"

namespace Editor
{
	class Editor;

	// ������ �ٱ����� ������ ��� �ȵȴ�.
	// => Spartan�� �ش� ���װ� ����.
	class Widget
	{
	public:
		Widget(Editor* pEditor);
		virtual ~Widget() = default;

		void Tick();

		// menubar, toolbar, progressbar�� �����Ѵ�.
		virtual void TickAlways() {}
		// �Ϲ����� �������� �����̴�.
		virtual void TickVisible() {}
		virtual void OnShow() {}
		virtual void OnHide() {}
		virtual void OnPushStyleVar() {}

		bool IsVisible() const { return m_bVisible; }
		void SetVisible(bool visible) { m_bVisible = visible; }
		float GetHeight() const { return m_height; }
		std::string GetTitle() const { return m_title; }
		ImGuiWindow* GetWindow() const { return m_window; }

	protected:
		bool m_bWindow = true;
		bool m_bVisible = true;
		int m_flags = ImGuiWindowFlags_NoCollapse;
		float m_height = 0.0f;
		float m_alpha = -1.0f;
		ImVec2 m_position = ImVec2(-1.0f, -1.0f);
		ImVec2 m_size = ImVec2(-1.0f, -1.0f);
		ImVec2 m_sizeMax = ImVec2(FLT_MAX, FLT_MAX);
		ImVec2 m_padding = ImVec2(-1.0f, -1.0f);
		std::string m_title = "Title";
		ImGuiWindow* m_window = nullptr;

		Editor* m_pEditor = nullptr;

	private:
		unsigned char m_varPushes = 0;
	};
}