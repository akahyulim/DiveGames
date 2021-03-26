#pragma once
#include "Object.h"

namespace Dive
{
	// base��.
	class Component : public Object
	{
	public:
		Component(size_t typeHash) : Object(typeHash) {}
		virtual ~Component() {}

		virtual void Update(float deltaTime) {}
	};
}