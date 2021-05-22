#pragma once
#include "DiveEngine.h"

namespace Sandbox
{
	class Sandbox : public Dive::Runtime
	{
	public:
		Sandbox() = default;
		~Sandbox() = default;

		void Initialize() override;

	private:
		void triangle_coloring();
		void triangle_texturing();

	private:
		Dive::RenderPath3D m_renderPath;

		// ��ġ�� �� ���ٴ�.
		Dive::Mesh* m_triangle;
	};
}