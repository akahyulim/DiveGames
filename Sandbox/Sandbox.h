#pragma once
#include "DiveEngine.h"

namespace Sandbox
{
	class Sandbox : public Dive::Runtime
	{
	public:
		Sandbox();
		~Sandbox();

		bool Initialize() override;

	private:
		void triangle_coloring();
		void triangle_texturing();

	private:
		Dive::RenderPath3D m_renderPath;

		// ��ġ�� �� ���ٴ�.
		Dive::Mesh* m_pTriangle;
	};
}