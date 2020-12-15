#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class GameObject;

	// 기존에는 Component를 상속한 Behavior을 상속했다.
	// Behavior은 enable 여부를 판단하는 Component 상속 클래스이다.
	class Renderable : public Component
	{
	public:
		explicit Renderable(Context* context, GameObject* owner);
		~Renderable();

	private:
	private:
		// Rendering에 필요한 Resouce들을 정리부터 하자.
	};
}