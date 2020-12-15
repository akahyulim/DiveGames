#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class GameObject;

	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component);

	public:
		explicit Transform(Context* context, GameObject* owner);
		~Transform();

		void OnAwake() override;
		void OnStart() override;

		void OnStop() override;
		void OnRemove() override;

		void OnUpdate() override;

	private:

	private:
	};
}