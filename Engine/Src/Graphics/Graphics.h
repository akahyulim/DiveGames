#pragma once
#include "Core/Subsystem.h"


namespace Dive
{
	class Context;

	class Graphics : public Subsystem
	{
	public:
		Graphics(Context* context);
		~Graphics();

		bool IsInitialized() const { return m_bInitialized; }

	private:
	private:
		bool m_bInitialized;
	};
}