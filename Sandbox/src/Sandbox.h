#pragma once
#include "dive.h"

namespace Dive
{
	class Sandbox : public Application
	{
	public:
		Sandbox();
		~Sandbox() override = default;

		void Setup() override;
		void Start() override;
		void Stop() override;

	private:
		void cameraControll();

	private:
		std::unique_ptr<RenderTexture> m_renderTarget;
		GameObject* m_camera;
	};
}