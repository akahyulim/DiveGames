#pragma once
#include "RenderPath.h"
#include "Scene.h"

namespace Dive
{
	// 엔진의 Default Rendering 경로이다.
	// Wicked는 여기에서 기능이 추가된? RenderPath3D_PathTracing까지 있다.
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