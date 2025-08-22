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

		float GetWidth() const { return m_width; }
		float GetHeight() const { return m_height; }

		bool IsVisible() const { return m_isVisible; }
		void SetVisible(bool visible) { m_isVisible = visible; }

	protected:
		virtual void renderContent() {}

	protected:
		std::string m_title;

		float m_width;
		float m_height;
		bool m_isVisible;
		int m_flags;

		Editor* m_editor;
	};
}
