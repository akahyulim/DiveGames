#include "DivePch.h"
#include "Dive_Object.h"
#include "Context.h"


namespace Dive
{
	Dive_Object::Dive_Object(Context * context)
		: m_context(context)
	{
		assert(m_context);
	}
}