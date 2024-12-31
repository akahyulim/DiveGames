#pragma once
#include <string>
#include "imgui.h"
#include "dive.h"

namespace Dive
{
	class Editor;

	class Panel
	{
	public:
		Panel(Editor* pEditor);
		virtual ~Panel() = default;
		
		void Draw();

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		bool IsVisible() const { return m_bVisible; }
		void SetVisible(bool visible) { m_bVisible = visible; }

	protected:
		virtual void drawView() {}

	protected:
		Editor* m_pEditor;

		std::string m_Title;

		float m_Width;
		float m_Height;
		bool m_bVisible;
	};
}