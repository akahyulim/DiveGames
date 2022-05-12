#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Material : public Resource
	{
	public:
		RESOURCE_CLASS_TYPE(Material)

		Material(const std::string& name = "", unsigned long long id = 0);
		virtual ~Material() = default;
	};
}