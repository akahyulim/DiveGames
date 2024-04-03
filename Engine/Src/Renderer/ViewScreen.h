#pragma once
#include "RendererDefs.h"		// eRenderPath��. ���� Renderer�� �����Ű��.
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