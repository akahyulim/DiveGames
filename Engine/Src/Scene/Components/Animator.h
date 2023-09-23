#pragma once
#include "Component.h"

namespace Dive
{
	class GameObject;
	class Animation;
	class Transform;

	class Animator : public Component
	{
	public:
		Animator(GameObject* pGameObject);
		~Animator() = default;

		
		Animation* GetCurrentAnimation() const { return m_pCurrentAnimation; }
		void SetAnimation(Animation* pAnim);

	private:
		Animation* m_pCurrentAnimation;
	};
}