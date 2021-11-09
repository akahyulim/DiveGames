#pragma once
#include "Src/Core/DiveEngine.h"

namespace dive
{
	// RenderPath_Legacy�� ��ӹ޴°� ���� �� ����.
	class RenderPath3D_Editor : public DiveEngine::RenderPath3D_Legacy
	{
	public:
		RenderPath3D_Editor();
		~RenderPath3D_Editor();

		//= OVERRIDE ================================
		void PreUpdate() override;
		void Update(float deltaTime) override;
		void Render() const override;
		void Compose() const override;
		//============================================

	private:

	private:
		// Editor�� View�� ���� dvRenderTexture�� �ʿ��ϴ�.

	};
}