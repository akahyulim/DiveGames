#pragma once
#include "RendererDefs.h"		// eRenderPath용. 추후 Renderer에 흡수시키자.
#include "Renderer.h"
#include "Math/Frustum.h"

namespace Dive
{
	class GameObject;
	class Scene;
	class Camera;
	class Renderable;
	class Light;
	class Mesh;
	class Material;

	enum class eRenderable
	{
		Opaque,
		Transparent,
		Light,
		Camera
	};

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

		void passOpaque();
		void passTransparent();

		void passShadowGen();
		void passOpaqueDraw();
		void passTransparentDraw();
		

	private:
		Scene* m_pActiveScene;
		Camera* m_pCamera;
		eRenderPath m_RenderPath;

		std::unordered_map<eRenderable, std::vector<GameObject*>> m_Renderables;

		// vs
		FrameBuffer m_cpuFrameBuffer;
		LightVSBuffer m_cpuLightVSBuffer;

		// ps
		MaterialBuffer m_cpuMaterialBuffer;
		CameraBuffer m_cpuCameraBuffer;
		LightBuffer m_cpuLightBuffer;

		Frustum m_Frustum;
	};
}