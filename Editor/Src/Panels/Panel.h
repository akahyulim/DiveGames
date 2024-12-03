#pragma once
#include <string>
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"
#include "imgui-docking/imgui_stdlib.h"
#include "dive.h"

namespace Dive
{
	class Editor;

	class Panel
	{
	public:
		Panel(Editor* pEditor);
		virtual ~Panel() = default;
		
		void Tick();

		float GetWidth() const { return m_Width; }
		float GetHeight() const { return m_Height; }

		bool IsVisible() const { return m_bVisible; }
		void SetVisible(bool visible) { m_bVisible = visible; }

	protected:
		// �׻� �׷����� �ϴ� ��(ex. MenuBar)
		virtual void renderAlways() {}
		// â ������ Panel
		virtual void renderWindow() {}

	protected:
		Editor* m_pEditor;

		std::string m_Title;

		float m_Width;
		float m_Height;
		bool m_bVisible;
		bool m_bWindow;
	};
}