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
		void initialize_ImGui();
		void beginDockSpace();
		void endDockSpace();
		void applyStyle() const;

		void projectDialog();

	private:
		std::vector<std::shared_ptr<Widget>> m_widgets;

		bool m_bMaximize;
		bool m_bDockSpace;

		bool m_bShowProjectDialog;

		std::string m_projectName;
	};
}