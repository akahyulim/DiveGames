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