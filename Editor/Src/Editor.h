#pragma once
#include "Dive.h"

namespace Dive
{
	class Editor
	{
	public:
		Editor();
		~Editor() = default;

		int Run();

		void OnPostRender();

	private:
		void initializeImGui();
		void renderImGui();

	private:
		std::unique_ptr<Dive::Engine> m_pEngine;
	};
}