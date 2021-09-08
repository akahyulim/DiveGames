#include "Component.h"
#include "GameObject.h"

namespace dive
{
	Component::Component(size_t  typeHash, GameObject* pGameObject, Transform* pTransform)
		: Object(typeHash)
	{
		m_pGameObject = pGameObject;
		m_pTransform = pTransform ? pTransform : pGameObject->GetTransform();
	}

	template <typename T>
	inline constexpr eComponentType Component::TypeToEnum() { return eComponentType::Unknown; }

	// Explicit template instantiation
#define REGISTER_COMPONENT(T, enumT) template<> eComponentType Component::TypeToEnum<T>() { return enumT; }

// To add a new component to the engine, simply register it here
// �� ��ũ�ΰ� ���� ����ǳ�...?
	// ����� �������� ����Ʈ ���� Ȯ���� �ȵǴµ�?
	REGISTER_COMPONENT(Transform, eComponentType::Transform)
	REGISTER_COMPONENT(Camera, eComponentType::Camera)
	REGISTER_COMPONENT(MeshRenderer, eComponentType::MeshRenderer)
	REGISTER_COMPONENT(Light, eComponentType::Light)
}