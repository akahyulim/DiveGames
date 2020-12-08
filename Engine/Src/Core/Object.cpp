#include "DivePch.h"
#include "Object.h"
#include "Context.h"


namespace Dive
{
	Object::Object(Context * context)
		: m_context(context)
	{
		assert(m_context);
	}
}