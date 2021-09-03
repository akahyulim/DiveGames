#pragma once
#include "DiveEngine.h"
#include "Widget.h"
#include "RenderPathEditor.h"
#include <vector>
#include <memory>

namespace Editor
{
	class Editor : public dive::Runtime
	{
	public:
		Editor();
		~Editor();

		bool Initialize() override;

		void Compose() override;

		void ModifyWindow(dive::eWindowModes mode, unsigned int width, unsigned int height, bool maximize) override;

	private:
		void initialize_ImGui();
		void beginDockSpace();
		void endDockSpace();
		void applyStyle() const;


	private:
		std::vector<std::shared_ptr<Widget>> m_Widgets;

		bool m_bDockSpace;

		bool m_bShowProjectDialog;

		RenderPathEditor m_RenderPathEditor;
	};
}