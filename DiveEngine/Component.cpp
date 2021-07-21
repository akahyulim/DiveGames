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
}