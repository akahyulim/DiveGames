#pragma once
#include "Time.h"

namespace Dive
{
	class Renderer;

	class Engine
	{
	public:
		Engine()	= default;
		~Engine()	= default;

		void Initialize();
		void Shutdown();


	private:
	
	private:
		Time m_Time;
	};
}