#pragma once
#include "DiveEngine.h"

namespace sandbox
{
	class Sandbox : public dive::Runtime
	{
	public:
		Sandbox();
		~Sandbox();

		bool Initialize() override;

	private:
		void triangle_coloring();
		void triangle_texturing();

	private:
		dive::RenderPath3D mRenderPath;

		// 위치가 좀 에바다.
		dive::Mesh* mTriangle;
	};
}