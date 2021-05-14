#pragma once
#include "Runtime.h"
#include "RenderPath3D.h"
#include "Widget.h"

#pragma comment(lib, "DiveEngine.lib")

namespace Studio
{
	class Studio : public Dive::Runtime
	{
	public:
		Studio();
		~Studio();

		void Initialize() override;

		void Render() override;

	private:
	private:
		Dive::RenderPath3D m_renderPath;
	};
}