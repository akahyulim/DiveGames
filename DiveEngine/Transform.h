#pragma once
#include "Component.h"

namespace Dive
{
	class Transform : public Component
	{
	public:
		Transform();
		~Transform();

		void Update(float deltaTime) override;

	private:
	private:
	};
}