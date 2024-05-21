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
		void passDepth();
		void passDepthNew();
		void passOpaqueDraw();
		void passTransparentDraw();

		void forwardRender();
		void deferredRender();

		

	private:
		Scene* m_pActiveScene;
		Camera* m_pCamera;
		eRenderPath m_RenderPath;

		std::unordered_map<eRenderable, std::vector<GameObject*>> m_Renderables;

		Frustum m_Frustum;
	};
}