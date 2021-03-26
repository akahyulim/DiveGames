#pragma once
#include "RenderPath.h"
#include "Scene.h"

namespace Dive
{
	// 얘는 Load와 Start가 없다.
	// 아마도 Renderer와 Scene에서 생성한 Resource를 사용하기 때문인 것 같다.
	class RenderPath3D : public RenderPath
	{
	public:

		//= OVERRIDE ================================
		void PreUpdate() override;
		void Update(float deltaTime) override;
		void Render() const override;
		void Compose() const override;
		//============================================

	private:
	private:
		Scene* m_scene = &Scene::GetGlobalScene();

		// visibility를 가진다.

		// gpu resource를 가진다?
	};
}