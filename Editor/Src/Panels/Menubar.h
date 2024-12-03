#pragma once
#include "Panel.h"

namespace Dive
{
	class Menubar : public Panel
	{
	public:
		Menubar(Editor* pEditor);
		~Menubar() = default;

		void renderAlways() override;

	private:
		void menuFile();
		void menuGameObject();
		void menuComponent();
		void menuTools();
		void menuWindow();
		void menuHelp();

	private:

	};
}