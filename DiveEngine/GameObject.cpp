#include "GameObject.h"
#include "Transform.h"

namespace Dive
{
	GameObject::GameObject()
		: Object(typeid(GameObject).hash_code())
	{
		// Transform을 함께 생성한다.
	}
}
