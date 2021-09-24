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

	private:
		dive::RenderPath3D_Legacy* m_pRenderPath;
	};
}