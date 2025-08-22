#pragma once
#include "View.h"

namespace Dive
{
	class WorldView : public View
	{
	public:
		WorldView(Editor* editor);
		~WorldView() override = default;

		float GetCameraSpeed() const { return m_cameraSpeed; }
		void SetCameraSpeed(float speed);
		void AddCameraSpeed(int delta);

	protected:
		void renderContent() override;

	private:
		void cameraControll();

	private:
		std::unique_ptr<RenderTexture> m_renderTarget;
		float m_cameraSpeed = 1.0f;
	};
}
