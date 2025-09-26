#pragma once
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "dive.h"

namespace Dive
{
	class Editor;

	class View
	{
	public:
		View(Editor* editor);
		virtual ~View() = default;
	
		void ComposeUI();

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		DirectX::XMUINT2 GetMousePosition() const;

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		virtual void renderContent() {}

	protected:
		std::string m_Title;

		float m_Width;
		float m_Height;
		bool m_IsVisible;
		int m_Flags;

		Editor* m_Editor;
	};
}
