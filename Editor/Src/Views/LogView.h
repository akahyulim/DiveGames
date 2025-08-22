#pragma once
#include "View.h"

namespace Dive
{
	class LogView : public View
	{
	public:
		LogView(Editor* editor);
		~LogView();

		void renderContent() override;

	private:
	};
}
