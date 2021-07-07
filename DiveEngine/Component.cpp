#include "Component.h"
#include "GameObject.h"

namespace dive
{
	Component::Component(size_t  typeHash, GameObject* gameObject, Transform* transform)
		: Object(typeHash)
	{
		mGameObject = gameObject;
		mTransform = transform ? transform : gameObject->GetTransform();
	}
}