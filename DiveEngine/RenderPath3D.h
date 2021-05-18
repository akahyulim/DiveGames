#pragma once
#include "RenderPath.h"
#include "Scene.h"

namespace Dive
{
	// ������ Default Rendering ����̴�.
	// Wicked�� ���⿡�� ����� �߰���? RenderPath3D_PathTracing���� �ִ�.
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

		// visibility�� ������.

		// gpu resource�� ������?
	};
}