#pragma once
#include "DiveEngine.h"

namespace dive
{
	class RenderPath_Deferred : public RenderPath
	{
	public:
		RenderPath_Deferred();
		~RenderPath_Deferred();

		//= OVERRIDE ================================
		void PreUpdate() override;
		void Update(float deltaTime) override;
		void Render() const override;
		void Compose() const override;
		//============================================

		void ResizeBuffers(unsigned int width = 0, unsigned int height = 0);

	private:

	private:
		// GBuffer Textures
	};
}