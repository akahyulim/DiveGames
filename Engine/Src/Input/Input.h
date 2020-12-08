#pragma once
#include "Core/Object.h"
#include "Core/CoreEvents.h"

namespace Dive
{
	class DiveContext;

	class Input : public Object
	{
		DIVE_OBJECT(Input, Object);

	public:
		Input(Context* context);
		~Input();

		void OnUpdate(const E_UPDATE* evnt);

	private:
	};
}