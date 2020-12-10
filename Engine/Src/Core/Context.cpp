#include "DivePch.h"
#include "Context.h"
#include "Object.h"
#include "DiveDefs.h"


namespace Dive
{
	Context::~Context()
	{
		Clear();
	}

	void Context::Clear()
	{
		for (auto& subsystem : m_subsystems)
		{
			subsystem.reset();
		}
		m_subsystems.clear();
	}
}