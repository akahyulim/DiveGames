#include "Camera.h"


namespace dive
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