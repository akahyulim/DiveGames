#include "DivePch.h"
#include "BoneRenderer.h"
#include "Transform.h"
#include "Scene/GameObject.h"
#include "Core/Log.h"

namespace Dive
{
	BoneRenderer::BoneRenderer(GameObject* pGameObject)
		: Component(pGameObject),
		m_pBone(nullptr)
	{
	}

	BoneRenderer::~BoneRenderer()
	{
	}

	void BoneRenderer::Draw() const
	{
		// �ϴ� ���⿡�� ����� �� ���� �� �ϴ�.
		// �ⲯ�ؾ� bone�� ���¸� ������ �޽÷� ���� ����� ����?
		// ������ ���� ��ȯ ����� ���� ���� ���̴��� �����Ͽ� �׷��� �Ѵٴ� �Ŵ�.
	}
}