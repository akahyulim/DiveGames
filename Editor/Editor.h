#pragma once
#include "Src/DiveEngine.h"
#include "Panel.h"
#include "RenderPathEditor.h"
#include <vector>
#include <memory>

namespace Dive
{
	class Editor : public Dive::Runtime
	{
	public:
		Editor();
		~Editor();

		bool Initialize() override;

		void Compose() override;

		void ModifyWindow(Dive::eWindowModes mode, unsigned int width, unsigned int height, bool maximize) override;

	private:
		void initialize_ImGui();
		void beginDockSpace();
		void endDockSpace();
		void applyStyle() const;


	private:
		std::vector<std::shared_ptr<Panel>> m_Panels;

		bool m_bDockSpace;

		bool m_bShowProjectDialog;

		RenderPathEditor m_RenderPathEditor;
	};
}