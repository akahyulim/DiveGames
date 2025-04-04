#pragma once
#include "imgui.h"
#include "dive.h"

namespace Dive
{
	class Editor;

	class View
	{
	public:
		View(Editor* editor);
		virtual ~View() = default;
		
		void Draw();

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		bool IsVisible() const { return m_IsVisible; }
		void SetVisible(bool visible) { m_IsVisible = visible; }

	protected:
		virtual void drawView() {}

	protected:
		std::string m_Title;

		float m_Width;
		float m_Height;
		bool m_IsVisible;
		int m_Flags;

		Editor* m_Editor;
	};
}
