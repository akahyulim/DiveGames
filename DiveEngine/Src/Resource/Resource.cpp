#include "DivePch.h"
#include "Resource.h"
#include "Renderer/Model.h"

namespace Dive 
{
	template<typename T>
	inline constexpr eResourceType Resource::TypeToEnum() { return eResourceType::Unknown; }

	template<typename T>
	inline constexpr void validate_resource_type() { static_assert(std::is_base_of<Resource, T>::value, "Resource를 상속하지 않은 타입입니다."); }
}

#define INSTANTIATE_TO_RESOURCE_TYPE(T, enumT) template<> Dive::eResourceType Dive::Resource::TypeToEnum<T>() { Dive::validate_resource_type<T>(); return enumT; }

INSTANTIATE_TO_RESOURCE_TYPE(Dive::Model, Dive::eResourceType::Model)