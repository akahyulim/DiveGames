#include "DivePch.h"
#include "Context.h"
#include "Dive_Object.h"
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
			SAFE_DELETE(subsystem);
		}
		m_subsystems.clear();
	}
}