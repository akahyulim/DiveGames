#include "divepch.h"
#include "Component.h"
#include "Scene/GameObject.h"

namespace Dive
{
	unsigned long long Component::GetInstanceID() const
	{
		DV_ASSERT(m_pGameObject != nullptr);
		return m_pGameObject->GetInstanceID();
	}

	template <typename T>
	inline constexpr eComponentType Component::TypeToEnum() { return eComponentType::Unknown; }

#define REGISTER_COMPONENT(T, enumT) template<> eComponentType Component::TypeToEnum<T>() { return enumT; }

	REGISTER_COMPONENT(Transform, eComponentType::Transform)
	REGISTER_COMPONENT(Camera, eComponentType::Camera)
//	REGISTER_COMPONENT(MeshRenderer, eComponentType::MeshRenderer)
//	REGISTER_COMPONENT(dvMeshRenderer, eComponentType::dvMeshRenderer)
//	REGISTER_COMPONENT(StaticMeshRenderer, eComponentType::StaticMeshRenderer)
//	REGISTER_COMPONENT(Light, eComponentType::Light)
}