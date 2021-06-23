#pragma once
#include "DiveEngine.h"
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"

namespace editor
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

		bool IsVisible() const { return mbVisible; }
		void SetVisible(bool visible) { mbVisible = visible; }
		float GetHeight() const { return mHeight; }
		std::string GetTitle() const { return mTitle; }
		ImGuiWindow* GetWindow() const { return mWindow; }

	protected:
		bool mbWindow = true;
		bool mbVisible = true;
		int mFlags = ImGuiWindowFlags_NoCollapse;
		float mHeight = 0.0f;
		float mAlpha = -1.0f;
		ImVec2 mPosition = ImVec2(-1.0f, -1.0f);
		ImVec2 mSize = ImVec2(-1.0f, -1.0f);
		ImVec2 mMaxSize = ImVec2(FLT_MAX, FLT_MAX);
		ImVec2 mPadding = ImVec2(-1.0f, -1.0f);
		std::string mTitle = "Title";
		ImGuiWindow* mWindow = nullptr;

		Editor* mpEditor = nullptr;

	private:
		unsigned char mVarPushes = 0;
	};
}