#include "Component.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Light.h"

namespace dive
{
	Component::Component(GameObject* pGameObject, Transform* pTransform)
	{
		m_pGameObject = pGameObject;
		m_pTransform = pTransform ? pTransform : pGameObject->GetTransform();
	}

	template <typename T>
	inline constexpr eComponentType Component::TypeToEnum() { return eComponentType::Unknown; }

	// Explicit template instantiation
#define REGISTER_COMPONENT(T, enumT) template<> eComponentType Component::TypeToEnum<T>() { return enumT; }

// To add a new component to the engine, simply register it here
// 이 매크로가 언제 실행되냐...?
	// 디버그 과정에서 포인트 지점 확인이 안되는데?
	REGISTER_COMPONENT(Transform, eComponentType::Transform)
	REGISTER_COMPONENT(Camera, eComponentType::Camera)
	REGISTER_COMPONENT(MeshRenderer, eComponentType::MeshRenderer)
	REGISTER_COMPONENT(Light, eComponentType::Light)
}