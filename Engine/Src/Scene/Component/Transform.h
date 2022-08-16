#pragma once
#include "Component.h"

namespace Dive
{
	class Context;

	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component)

	public:
		Transform(Context* pContext);
		~Transform();

	private:
	private:
	};
}
