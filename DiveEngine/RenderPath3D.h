#pragma once
#include "RenderPath.h"
#include "SceneManager.h"
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

		// visibility를 가진다.

		// gpu resource를 가진다?
	};
}