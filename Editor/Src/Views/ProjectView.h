#pragma once
#include "View.h"

namespace Dive
{
	class ProjectView : public View
	{
	public:
		ProjectView(Editor* editor);
		~ProjectView();

		void drawView() override;

	private:
	};
}