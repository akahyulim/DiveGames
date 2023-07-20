#pragma once
#include "Core/Object.h"

namespace Dive
{
	class Camera;
	class Drawable;
	class Light;
	class GBuffer;
	class IMeshRenderer;

	enum class eRenderPath
	{
		Forward,
		Deferred,
	};

	class View : public Object
	{
	public:
		View(eRenderPath path = eRenderPath::Forward);
		View(Camera* pCamera, eRenderPath path = eRenderPath::Forward);
		~View();

		void Update();
		void Render();

		eRenderPath GetRenderPath() const { return m_RenderPath; }
		void SetRenderPath(eRenderPath path) { m_RenderPath = path; }

		Camera* GetCamera() const { return m_pCamera; }
		void SetCamera(Camera* pCamera) { m_pCamera = pCamera; }

	private:
		void renderPathEndeavor();

	private:
		Camera* m_pCamera;
		std::vector<Drawable*> m_Drawables;
		std::vector<Light*> m_Lights;

		eRenderPath m_RenderPath;

		GBuffer* m_pGBuffer;

		std::vector<IMeshRenderer*> m_MeshRenderers;
	};
}