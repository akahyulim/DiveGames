#pragma once
#include "RenderPath.h"
#include "Scene.h"

// Basic class for 3d rendering paths.
// The post proecess chain is also implemented here.
namespace dive
{
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
		Scene* m_pScene = &Scene::GetGlobalScene();

		// visibility�� ������.

		// gpu resource�� ������?
	};
}