#pragma once
#include "DiveEngine.h"

namespace Dive
{
	class Editor : public Runtime
	{
	public:
		Editor() = default;
		~Editor();

	private:

	private:
	};

	Runtime* CreateApplication(HINSTANCE hInstance);
}