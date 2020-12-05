#pragma once
#include "Core/Dive_Object.h"

namespace Dive
{
	class DiveContext;

	class Input : public Dive_Object
	{
		NEW_DIVE_OBJECT(Input);

	public:
		Input(Dive_Context* context);

		void OnUpdate(size_t eventType);

	private:
	};
}