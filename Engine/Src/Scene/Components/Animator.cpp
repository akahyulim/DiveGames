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

	// ���� �ټ��� Animation�� ����ϰ� ��ȯ�� �� �־�� �Ѵ�.
	// �׸��� �� ��ȯ�������� ������ �̷������ �Ѵ�.
	void Animator::SetAnimation(Animation* pAnim)
	{
		m_pCurrentAnimation = pAnim;
	}
}