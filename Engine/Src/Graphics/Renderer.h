#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	
	class Renderer : public Object
	{
		DIVE_OBJECT(Renderer, Object);

	public:
		Renderer(Context* context);
		~Renderer();

	
	private:

	private:
	};
}

