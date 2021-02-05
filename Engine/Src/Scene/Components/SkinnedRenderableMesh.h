#pragma once
#include "RenderableMesh.h"

namespace Dive
{
	class Transform;

	class SkinnedRenderableMesh : public RenderableMesh
	{
		DIVE_OBJECT(SkinnedRenderableMesh, RenderableMesh);

	public:
		SkinnedRenderableMesh(Context* context, GameObject* owner);
		~SkinnedRenderableMesh();

		bool SetMesh(Mesh* mesh) override;

		Transform* GetRootBone() const		{ return m_rootBone; }
		void SetRootBone(Transform* bone)	{ m_rootBone = bone; }

	private:
	private:
		Transform* m_rootBone;
	};
}