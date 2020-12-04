#pragma once
#include "Core/Subsystem.h"

namespace Dive
{
	class Context;

	class Input : public Subsystem
	{
	public:
		Input(Context* context);

		void Update(size_t eventType);

	private:
	};
}