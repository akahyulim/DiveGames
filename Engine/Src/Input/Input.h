#pragma once
#include "Core/Dive_Object.h"
#include "Core/CoreEvents.h"

namespace Dive
{
	class DiveContext;

	class Input : public Dive_Object
	{
		NEW_DIVE_OBJECT(Input);

	public:
		Input(Context* context);
		~Input();

		void OnUpdate(const E_UPDATE* evnt);

	private:
	};
}