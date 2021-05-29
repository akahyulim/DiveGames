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
		void initialized_ini();
		void initialize_ImGui();
		void begin_ImGui();
		void end_ImGui();

	private:
		std::string m_iniFilePath;
		std::vector<std::shared_ptr<Widget>> m_widgets;

		bool m_bMaximize;
		bool m_editor_begun;
	};
}