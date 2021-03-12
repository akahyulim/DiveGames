#pragma once
#include "RenderPath.h"
#include "Scene.h"

namespace Dive
{
	class RenderPath3D : public RenderPath
	{
	public:

		//= OVERRIDE ================================
		void PreUpdate() override;
		void Update(float deltaTime) override;
		void Render() const override;
		void Compose(CommandList cmd) const override;
		//============================================

	private:
	private:
		Scene* m_scene = &Scene::GetGlobalScene();

		// gpu resource를 가진다?
	};
}