#include "GameObject.h"
#include "Transform.h"

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code())
	{
		// Transform�� �Բ� �����Ѵ�.
	}
}
