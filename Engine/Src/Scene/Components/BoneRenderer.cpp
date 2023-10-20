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
		// 일단 여기에서 계산할 건 없을 듯 하다.
		// 기껏해야 bone의 형태를 스퀘어 메시로 직접 만드는 정도?
		// 문제는 최종 변환 행렬을 얻어와 전용 쉐이더에 전달하여 그려야 한다는 거다.
	}
}