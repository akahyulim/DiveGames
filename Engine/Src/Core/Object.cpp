#include "DivePch.h"
#include "Object.h"

namespace Dive
{
	Object::Object()
		: m_ID(0)
	{}

	Object::Object(const std::string& name)
		: m_ID(0)
		, m_Name(name)
	{}
}