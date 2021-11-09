#pragma once
#include "Src/Core/DiveEngine.h"

namespace sandbox
{
	class Sandbox : public DiveEngine::Runtime
	{
	public:
		Sandbox();
		~Sandbox();

		bool Initialize() override;

	private:

	private:
		DiveEngine::RenderPath3D_Legacy* m_pRenderPath;
	};
}