#include "divepch.h"
#include "Resource.h"

namespace Dive
{
	Resource::Resource(const std::string& name, unsigned long long id)
		: Object(name, id)
	{ 
	}
}