#pragma once
#include "DiveEngine.h"

namespace Editor
{
	class Editor : public Dive::Runtime
	{
	public:
		Editor();
		~Editor();

		void Initialize() override;

	private:
	private:
	};
}