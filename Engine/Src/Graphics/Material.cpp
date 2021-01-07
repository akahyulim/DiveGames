#include "DivePch.h"
#include "Material.h"
#include "Core/Context.h"
#include "Core/Log.h"


namespace Dive
{
	Material::Material(Context * context)
		: Resource(context, eResourceType::Material)
	{
	}

	Material::~Material()
	{
	}
}