#pragma once
#include "RendererDefs.h"		// eRenderPath��. ���� Renderer�� �����Ű��.
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

	// �ƹ����� �̰� ����Ƽ Ȥ�� sparky�� Layer�� ��︰��.
	// ���� �� �����غ���.
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