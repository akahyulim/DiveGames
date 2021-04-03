#pragma once

namespace Sandbox
{
	class Sandbox : public Dive::Runtime
	{
	public:
		Sandbox() = default;
		~Sandbox() = default;

		void Initialize() override;

	private:
	private:
		Dive::RenderPath3D m_renderPath;

		// 위치가 좀 에바다.
		Dive::Mesh* m_triangle;
	};
}