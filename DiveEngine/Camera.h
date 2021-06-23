#pragma once
#include "Component.h"


namespace dive
{
	class Camera : public Component
	{
	public:
		Camera();
		~Camera();

		void Update(float deltaTime) override;

	private:
	};
}