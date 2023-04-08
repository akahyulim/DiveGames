#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Camera;
	class Drawable;

	class View : public Object
	{
	public:
		View();
		View(Camera* pCamera);
		~View();

		void Update(float delta);
		void Render();

		Camera* GetCamera() const { return m_pCamera; }
		void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

	private:
	private:
		Camera* m_pCamera;
		std::vector<Drawable*> m_Drawables;
	};
}