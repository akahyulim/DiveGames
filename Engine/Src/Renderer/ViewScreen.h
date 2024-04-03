#pragma once
#include "RendererDefs.h"		// eRenderPath용. 추후 Renderer에 흡수시키자.
#include "Renderer.h"
#include "Math/Frustum.h"

namespace Dive
{
	class Scene;
	class Camera;
	class Renderable;
	class Light;
	class Mesh;
	class Material;

	// 아무래도 이건 유니티 혹은 sparky의 Layer가 어울린다.
	// 추후 더 생각해보자.
	class ViewScreen
	{
	public:
		ViewScreen(Camera* pCamera, eRenderPath renderPath = eRenderPath::Forward);

		void Update();
		void Render();

	private:
		void forwardRender();
		void deferredRender();

	private:
		Scene* m_pActiveScene;
		Camera* m_pCamera;
		eRenderPath m_RenderPath;

		std::vector<Renderable*> m_Renderables;
		std::vector<Light*> m_Lights;

		FrameBuffer m_cpuFrameBuffer;
		MaterialBuffer m_cpuMaterialBuffer;
		CameraBuffer m_cpuCameraBuffer;
		LightBuffer m_cpuLightBuffer;

		Frustum m_Frustum;
	};
}