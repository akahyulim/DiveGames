#include "DivePch.h"
#include "Resource.h"
#include "Image.h"
#include "Renderer/Model.h"
#include "Renderer/Material.h"
#include "Renderer/Animation.h"
#include "Graphics/Texture2D.h"
#include "Graphics/RenderTexture.h"

#include "Graphics/DvTexture2D.h"
#include "Graphics/DvRenderTexture.h"
#include "Graphics/DvShader.h"
#include "Renderer/DvMesh.h"
#include "Renderer/DvMaterial.h"

namespace Dive 
{
	template<typename T>
	inline constexpr eResourceType Resource::TypeToEnum() { return eResourceType::Unknown; }

	template<typename T>
	inline constexpr void validate_resource_type() { static_assert(std::is_base_of<Resource, T>::value, "Resource를 상속하지 않은 타입입니다."); }
}

#define INSTANTIATE_TO_RESOURCE_TYPE(T, enumT) template<> Dive::eResourceType Dive::Resource::TypeToEnum<T>() { Dive::validate_resource_type<T>(); return enumT; }

INSTANTIATE_TO_RESOURCE_TYPE(Dive::Model, Dive::eResourceType::Model)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Material, Dive::eResourceType::Material)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Animation, Dive::eResourceType::Animation)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Image, Dive::eResourceType::Image)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::Texture2D, Dive::eResourceType::Texture2D)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::RenderTexture, Dive::eResourceType::RenderTexture)

INSTANTIATE_TO_RESOURCE_TYPE(Dive::DvTexture2D, Dive::eResourceType::DvTexture2D)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::DvRenderTexture, Dive::eResourceType::DvRenderTexture)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::DvShader, Dive::eResourceType::DvShader)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::DvMesh, Dive::eResourceType::DvMesh)
INSTANTIATE_TO_RESOURCE_TYPE(Dive::DvMaterial, Dive::eResourceType::DvMaterial)