#include "Resource.h"
#include "../Renderer/Graphics/dvTexture2D.h"

namespace dive
{
	Resource::Resource()
	{
	}

	template <typename T>
	inline constexpr eResourceType Resource::TypeToEnum() { return eResourceType::Unknown; }

#define INSTANTIATE_TO_RESOURCE_TYPE(T, enumT) template<> eResourceType Resource::TypeToEnum<T>() { return enumT; }

	INSTANTIATE_TO_RESOURCE_TYPE(dvTexture2D, eResourceType::Texture2D)
}