#pragma once
#include "Panel.h"

namespace Dive
{
	class ProjectView : public Panel
	{
	public:
		ProjectView(Editor* pEditor);
		virtual ~ProjectView();

		void drawView() override;

	private:
	private:
	};
}