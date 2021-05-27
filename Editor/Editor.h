#pragma once
#include "DiveEngine.h"
#include "Widget.h"
#include <vector>
#include <memory>

namespace Editor
{
	class Editor : public Dive::Runtime
	{
	public:
		Editor();
		~Editor();

		bool Initialize() override;

		void Render() override;

		void ResizeWindow(unsigned int width, unsigned int height);

	private:
	private:
		std::vector<std::shared_ptr<Widget>> m_widgets;
	};

	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}