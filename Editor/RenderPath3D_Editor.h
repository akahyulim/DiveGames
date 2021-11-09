#pragma once
#include "Src/Core/DiveEngine.h"

namespace dive
{
	// RenderPath_Legacy를 상속받는게 맞을 것 같다.
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
		// Editor의 View에 붙일 dvRenderTexture가 필요하다.

	};
}