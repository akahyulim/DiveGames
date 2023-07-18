#pragma once
#include "MeshRenderer.h"

namespace Dive
{
	class IMesh;
	class SkinnedMesh;

	class Animation;
	class Animator;

	class SkinnedMeshRenderer : public IMeshRenderer
	{
	public:
		SkinnedMeshRenderer(GameObject* pGameObject);
		virtual ~SkinnedMeshRenderer();

		void Draw() const override;

		IMesh* GetMesh() override;
		void SetMesh(IMesh* pMesh) override;

		void SetAnimation(Animation* pAnim);


	private:
	private:
		SkinnedMesh* m_pMesh;

		Animator* m_pAnimator;
	};
}