#include "Component.h"
#include "GameObject.h"

namespace dive
{
	Component::Component(size_t  typeHash, GameObject* owner, Transform* transform)
		: Object(typeHash)
	{
		mOwner = owner;
		mTransform = transform ? transform : owner->GetTransform();
	}
}