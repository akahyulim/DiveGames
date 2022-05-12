#include "divepch.h"
#include "Material.h"

namespace Dive
{
	Material::Material(const std::string& name, unsigned long long id)
		: Resource(name, id)
	{
	}
}