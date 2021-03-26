#pragma once
#include "Component.h"


namespace Dive
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