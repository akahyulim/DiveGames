#pragma once
#include "DiveEngine.h"
#include "Widget.h"
#include "RenderPathEditor.h"
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

		void ModifyWindow(Dive::eWindowModes mode, unsigned int width, unsigned int height, bool maximize) override;

	private:
		void initialize_ImGui();
		void beginDockSpace();
		void endDockSpace();
		void applyStyle() const;


	private:
		std::vector<std::shared_ptr<Widget>> m_widgets;

		bool m_bDockSpace;

		bool m_bShowProjectDialog;

		RenderPathEditor m_renderPathEditor;
	};
}