#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Context;
	class Graphics;
	
	class Renderer : public Object
	{
		DIVE_OBJECT(Renderer, Object);

	public:
		Renderer(Context* context);
		~Renderer();

		void Initialize();
		void Render();

		bool IsInitialized() const { return m_bInitialized; }

	private:

	private:
		std::weak_ptr<Graphics> m_graphics;

		bool m_bInitialized;
	};
}

