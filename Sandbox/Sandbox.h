#pragma once
#include "DiveEngine.h"

namespace Sandbox
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
		dive::RenderPath3D m_renderPath;

		// ��ġ�� �� ���ٴ�.
		dive::Mesh* m_pTriangle;
	};
}