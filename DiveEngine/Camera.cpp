#include "Camera.h"


namespace Dive
{
	Camera::Camera()
		: Component(typeid(Camera).hash_code())
	{
	}
	
	Camera::~Camera()
	{
	}

	void Camera::Update(float deltaTime)
	{
	}
}