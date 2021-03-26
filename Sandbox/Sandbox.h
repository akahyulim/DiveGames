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
	};
}