#pragma once
#include "Widget.h"

namespace Editor
{
	class Scene : public Widget
	{
	public:
		Scene(Editor* pEditor);

		void TickVisible() override;

	private:
	private:
	};
}