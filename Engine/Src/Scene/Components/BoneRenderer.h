#pragma once
#include "Component.h"
#include "Renderer/Skeleton.h"

namespace Dive
{
	class GameObject;

	class BoneRenderer : public Component
	{
	public:
		BoneRenderer(GameObject* pGameObject);
		virtual ~BoneRenderer();

		void Draw() const;

		void SetBone(const BoneInfo* pBone) { m_pBone = pBone; }
		const BoneInfo* GetBone() const { return m_pBone; }

	private:
	private:
		const BoneInfo* m_pBone;


	};
}