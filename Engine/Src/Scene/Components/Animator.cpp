#include "DivePch.h"
#include "Animator.h"
#include "Transform.h"
#include "BoneRenderer.h"
#include "Renderer/Animation.h"
#include "Renderer/Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/ConstantBuffer.h"
#include "Core/Log.h"

namespace Dive
{
	Animator::Animator(GameObject* pGameObject)
		: Component(pGameObject),
		m_pCurrentAnimation(nullptr)
	{
	}

	// 추후 다수의 Animation을 등록하고 전환할 수 있어야 한다.
	// 그리고 이 전환과정에서 보간도 이루어져야 한다.
	void Animator::SetAnimation(Animation* pAnim)
	{
		m_pCurrentAnimation = pAnim;
	}
}