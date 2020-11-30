#pragma once
#include "DivePch.h"

namespace Dive
{
	class Context;

	class Subsystem
	{
	public:
		explicit Subsystem(Context* context)
			: m_context(context) {}
		virtual ~Subsystem() = default;

	protected:
	protected:
		Context* m_context;
	};
}