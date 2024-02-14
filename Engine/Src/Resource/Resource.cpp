#include "DivePch.h"
#include "Resource.h"
#include "Image.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

namespace Dive 
{
	template<typename T>
	inline constexpr eResourceType Resource::TypeToEnum() { return eResourceType::Unknown; }

	template<typename T>
	inline constexpr void validate_resource_type() { static_assert(std::is_base_of<Resource, T>::value, "Resource를 상속하지 않은 타입입니다."); }
}

#define INSTANTIATE_TO_RESOURCE_TYPE(T, enumT) template<> Dive::eResourceType Dive::Resource::TypeToEnum<T>() { Dive::validate_resource_type<T>(); return enumT; }

INSTANTIATE_TO_RESOURCE_TYPE(Dive::Image, Dive::eResourceType::Image)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Texture2D, Dive::eResourceType::Texture2D)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::RenderTexture, Dive::eResourceType::RenderTexture)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Shader, Dive::eResourceType::Shader)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Mesh, Dive::eResourceType::Mesh)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Material, Dive::eResourceType::Material)