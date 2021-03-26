#include "Transform.h"


namespace Dive
{
	Transform::Transform()
		: Component(typeid(Transform).hash_code())
	{

	}

	Transform::~Transform()
	{
	}

	void Transform::Update(float deltaTime)
	{
	}
}