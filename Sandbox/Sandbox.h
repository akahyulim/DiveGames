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

		// ��ġ�� �� ���ٴ�.
		dive::Mesh* mTriangle;
	};
}