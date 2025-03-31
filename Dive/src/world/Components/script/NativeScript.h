#pragma once
#include "../Component.h"

namespace Dive
{
	class NativeScript : public Component
	{
		DV_CLASS(NativeScript, Component)
			
	public:
		NativeScript(GameObject* gameObject) : Component(gameObject) {}
		~NativeScript() override = default;

		virtual void Create() {}
		virtual void Update() {}
		virtual void Destroy() {}

		virtual void OnCollision(GameObject* other) {}
		virtual void OnTrigger(GameObject* other) {}
		virtual void OnKeyPress(int keyCode) {}
	};
}
