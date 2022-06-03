#include "divepch.h"
#include "Resource.h"

namespace Dive
{
	Resource::Resource()
	{
	}

	Resource::Resource(unsigned long long id)
		: Object(id)
	{
	}
	
	//Resource::Resource(const std::string& name, unsigned long long id)
	//	: Object(name, id)
	//{ 
	//}
}